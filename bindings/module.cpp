#include <algorithm>
#include <cctype>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

#include <windows.h>
#include <commdlg.h>

#include <pybind11/functional.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include "truegraphics/core/App.h"
#include "truegraphics/resource/Resources.h"
#include "truegraphics/state/State.h"
#include "truegraphics/widgets/Button.h"
#include "truegraphics/widgets/Canvas.h"
#include "truegraphics/widgets/Checkbox.h"
#include "truegraphics/widgets/RadioButton.h"
#include "truegraphics/widgets/Dropdown.h"
#include "truegraphics/widgets/Container.h"
#include "truegraphics/widgets/Image.h"
#include "truegraphics/widgets/Label.h"
#include "truegraphics/widgets/ListView.h"
#include "truegraphics/widgets/PasswordBox.h"
#include "truegraphics/widgets/ProgressBar.h"
#include "truegraphics/widgets/ScrollView.h"
#include "truegraphics/widgets/Slider.h"
#include "truegraphics/widgets/Table.h"
#include "truegraphics/widgets/Tabs.h"
#include "truegraphics/widgets/TextArea.h"
#include "truegraphics/widgets/TextBox.h"
#include "truegraphics/widgets/TreeView.h"
#include "truegraphics/widgets/Widget.h"

namespace py = pybind11;

namespace {

thread_local std::vector<std::weak_ptr<truegraphics::widgets::Widget>> g_parent_stack;

truegraphics::graphics::Color parse_color(const py::object& obj) {
  if (py::isinstance<py::str>(obj)) {
    std::string s = obj.cast<std::string>();
    if (!s.empty() && s[0] == '#') {
      s.erase(s.begin());
    }
    if (s.size() != 6 && s.size() != 8) {
      throw std::runtime_error("Color hex strings must be #RRGGBB or #RRGGBBAA");
    }
    for (char c : s) {
      if (!std::isxdigit(static_cast<unsigned char>(c))) {
        throw std::runtime_error("Invalid hex digit in color string");
      }
    }
    auto hex_byte = [&](size_t i) -> unsigned char {
      return static_cast<unsigned char>(std::stoi(s.substr(i, 2), nullptr, 16));
    };
    unsigned char r = hex_byte(0);
    unsigned char g = hex_byte(2);
    unsigned char b = hex_byte(4);
    unsigned char a = s.size() == 8 ? hex_byte(6) : 255;
    return {r, g, b, a};
  }

  if (py::isinstance<py::sequence>(obj) && !py::isinstance<py::bytes>(obj)) {
    py::sequence seq = obj.cast<py::sequence>();
    if (py::len(seq) != 3 && py::len(seq) != 4) {
      throw std::runtime_error("Color sequences must be (r,g,b) or (r,g,b,a)");
    }
    int r = seq[0].cast<int>();
    int g = seq[1].cast<int>();
    int b = seq[2].cast<int>();
    int a = py::len(seq) == 4 ? seq[3].cast<int>() : 255;
    return {
        static_cast<unsigned char>(std::clamp(r, 0, 255)),
        static_cast<unsigned char>(std::clamp(g, 0, 255)),
        static_cast<unsigned char>(std::clamp(b, 0, 255)),
        static_cast<unsigned char>(std::clamp(a, 0, 255)),
    };
  }

  throw std::runtime_error("Unsupported color type (use '#RRGGBB' or (r,g,b[,a]))");
}

std::shared_ptr<truegraphics::widgets::Widget> current_parent() {
  while (!g_parent_stack.empty()) {
    if (auto p = g_parent_stack.back().lock()) {
      return p;
    }
    g_parent_stack.pop_back();
  }
  return nullptr;
}

void push_parent(const std::shared_ptr<truegraphics::widgets::Widget>& parent) { g_parent_stack.push_back(parent); }

void pop_parent(const std::shared_ptr<truegraphics::widgets::Widget>& parent) {
  if (g_parent_stack.empty()) {
    return;
  }
  if (auto top = g_parent_stack.back().lock()) {
    if (top.get() == parent.get()) {
      g_parent_stack.pop_back();
      return;
    }
  }
  // Best-effort: remove first matching parent from the end.
  for (auto it = g_parent_stack.rbegin(); it != g_parent_stack.rend(); ++it) {
    if (auto p = it->lock(); p && p.get() == parent.get()) {
      g_parent_stack.erase(std::next(it).base());
      return;
    }
  }
}

template <typename T, typename... Args>
std::shared_ptr<T> create_and_attach(Args&&... args) {
  auto* app = truegraphics::core::current_app();
  if (!app) {
    throw std::runtime_error("Widgets must be created inside an active @app.window / app.add_window build scope.");
  }
  auto widget = std::make_shared<T>(std::forward<Args>(args)...);
  app->attach_widget(widget, current_parent());
  return widget;
}

}  // namespace

PYBIND11_MODULE(_truegraphics, m) {
  m.doc() = "TrueGraphics native engine bindings";

  m.def("add_resource_path", &truegraphics::resource::add_path, py::arg("path"));
  m.def("clear_resource_paths", &truegraphics::resource::clear_paths);
  m.def("resource_paths", &truegraphics::resource::paths);
  m.def("resolve_resource", &truegraphics::resource::resolve, py::arg("path"));
  m.def("load_font", &truegraphics::resource::load_font, py::arg("path"));
  m.def("unload_font", &truegraphics::resource::unload_font, py::arg("path"));

  m.def("clipboard_set_text", [](const std::string& text) {
    if (!OpenClipboard(nullptr)) {
      throw std::runtime_error("OpenClipboard failed");
    }
    EmptyClipboard();

    HGLOBAL hmem = GlobalAlloc(GMEM_MOVEABLE, text.size() + 1);
    if (!hmem) {
      CloseClipboard();
      throw std::runtime_error("GlobalAlloc failed");
    }

    void* ptr = GlobalLock(hmem);
    if (!ptr) {
      GlobalFree(hmem);
      CloseClipboard();
      throw std::runtime_error("GlobalLock failed");
    }
    memcpy(ptr, text.c_str(), text.size() + 1);
    GlobalUnlock(hmem);

    if (!SetClipboardData(CF_TEXT, hmem)) {
      GlobalFree(hmem);
      CloseClipboard();
      throw std::runtime_error("SetClipboardData failed");
    }

    CloseClipboard();
  });

  m.def("clipboard_get_text", []() -> std::string {
    if (!OpenClipboard(nullptr)) {
      throw std::runtime_error("OpenClipboard failed");
    }

    HANDLE h = GetClipboardData(CF_TEXT);
    if (!h) {
      CloseClipboard();
      return {};
    }

    char* ptr = static_cast<char*>(GlobalLock(h));
    if (!ptr) {
      CloseClipboard();
      return {};
    }

    std::string out(ptr);
    GlobalUnlock(h);
    CloseClipboard();
    return out;
  });

  m.def("message_box", [](const std::string& text, const std::string& title) {
    return MessageBoxA(nullptr, text.c_str(), title.c_str(), MB_OK | MB_ICONINFORMATION);
  }, py::arg("text"), py::arg("title") = "TrueGraphics");

  m.def("open_file_dialog", []() -> std::string {
    wchar_t filename[MAX_PATH] = L"";
    OPENFILENAMEW ofn{};
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = nullptr;
    ofn.lpstrFile = filename;
    ofn.nMaxFile = MAX_PATH;
    ofn.lpstrFilter = L"All Files\0*.*\0";
    ofn.nFilterIndex = 1;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

    if (GetOpenFileNameW(&ofn)) {
      std::wstring ws(filename);
      return std::string(ws.begin(), ws.end());
    }
    return {};
  });

  m.def("save_file_dialog", []() -> std::string {
    wchar_t filename[MAX_PATH] = L"";
    OPENFILENAMEW ofn{};
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = nullptr;
    ofn.lpstrFile = filename;
    ofn.nMaxFile = MAX_PATH;
    ofn.lpstrFilter = L"All Files\0*.*\0";
    ofn.nFilterIndex = 1;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR;

    if (GetSaveFileNameW(&ofn)) {
      std::wstring ws(filename);
      return std::string(ws.begin(), ws.end());
    }
    return {};
  });

  py::enum_<truegraphics::widgets::Container::LayoutMode>(m, "LayoutMode")
      .value("None", truegraphics::widgets::Container::LayoutMode::None)
      .value("Column", truegraphics::widgets::Container::LayoutMode::Column)
      .value("Row", truegraphics::widgets::Container::LayoutMode::Row)
      .value("Grid", truegraphics::widgets::Container::LayoutMode::Grid);

  py::enum_<truegraphics::widgets::Container::Align>(m, "Align")
      .value("Start", truegraphics::widgets::Container::Align::Start)
      .value("Center", truegraphics::widgets::Container::Align::Center)
      .value("End", truegraphics::widgets::Container::Align::End)
      .value("Stretch", truegraphics::widgets::Container::Align::Stretch);

  py::enum_<truegraphics::widgets::Container::Justify>(m, "Justify")
      .value("Start", truegraphics::widgets::Container::Justify::Start)
      .value("Center", truegraphics::widgets::Container::Justify::Center)
      .value("End", truegraphics::widgets::Container::Justify::End)
      .value("SpaceBetween", truegraphics::widgets::Container::Justify::SpaceBetween);

  py::class_<truegraphics::core::App>(m, "App")
      .def(py::init<>())
      .def("run", &truegraphics::core::App::run)
      .def("set_title", &truegraphics::core::App::set_title)
      .def("set_size", &truegraphics::core::App::set_size)
      .def("set_fullscreen", &truegraphics::core::App::set_fullscreen)
      .def("set_icon", &truegraphics::core::App::set_icon)
      .def("set_min_size", &truegraphics::core::App::set_min_size)
      .def("set_max_size", &truegraphics::core::App::set_max_size)
      .def("set_theme", [](truegraphics::core::App& self, const std::string& theme) {
        if (theme == "dark") {
          self.set_theme(true);
        } else if (theme == "light") {
          self.set_theme(false);
        } else {
          throw std::runtime_error("theme must be 'dark' or 'light'");
        }
      })
      .def("add_window", [](truegraphics::core::App& self, py::function fn, const std::string& title, int width, int height) {
        return self.add_window(title, width, height, [fn = std::move(fn)]() {
          py::gil_scoped_acquire gil;
          fn();
        });
      }, py::arg("fn"), py::arg("title") = "TrueGraphics", py::arg("width") = 640, py::arg("height") = 480)
      .def("enable_tray", [](truegraphics::core::App& self, const std::string& icon_path, const std::string& tooltip, py::object on_click) {
        std::function<void()> cb;
        if (!on_click.is_none()) {
          py::function f = on_click.cast<py::function>();
          cb = [f = std::move(f)]() {
            py::gil_scoped_acquire gil;
            f();
          };
        }
        self.enable_tray(icon_path, tooltip, std::move(cb));
      }, py::arg("icon_path"), py::arg("tooltip") = "TrueGraphics", py::arg("on_click") = py::none())
      .def("disable_tray", &truegraphics::core::App::disable_tray)
      .def("set_timeout", [](truegraphics::core::App& self, double seconds, py::function cb) {
        return self.set_timeout(seconds, [cb = std::move(cb)]() {
          py::gil_scoped_acquire gil;
          cb();
        });
      })
      .def("set_interval", [](truegraphics::core::App& self, double seconds, py::function cb) {
        return self.set_interval(seconds, [cb = std::move(cb)]() {
          py::gil_scoped_acquire gil;
          cb();
        });
      })
      .def("clear_timer", &truegraphics::core::App::clear_timer)
      .def("post", [](truegraphics::core::App& self, py::function cb) {
        self.post([cb = std::move(cb)]() {
          py::gil_scoped_acquire gil;
          cb();
        });
      })
      .def("run_async", [](truegraphics::core::App& self, py::function task, py::object on_done) {
        std::function<void()> done_cb;
        if (!on_done.is_none()) {
          py::function done = on_done.cast<py::function>();
          done_cb = [done = std::move(done)]() {
            py::gil_scoped_acquire gil;
            done();
          };
        }

        self.run_async(
            [task = std::move(task)]() {
              py::gil_scoped_acquire gil;
              task();
            },
            std::move(done_cb));
      },
      py::arg("task"),
      py::arg("on_done") = py::none())
      .def("on_shortcut", [](truegraphics::core::App& self, const std::string& combo, py::function cb) {
        self.on_shortcut(combo, [cb = std::move(cb)]() {
          py::gil_scoped_acquire gil;
          cb();
        });
      })
      .def(
          "window",
          [](truegraphics::core::App& self, py::function fn) {
            self.set_builder([fn]() {
              py::gil_scoped_acquire gil;
              fn();
            });
            return fn;
          },
          py::return_value_policy::move);

  py::class_<truegraphics::widgets::Widget, std::shared_ptr<truegraphics::widgets::Widget>>(m, "Widget")
      .def("set_id", &truegraphics::widgets::Widget::set_id)
      .def("set_text", &truegraphics::widgets::Widget::set_text)
      .def("set_size", &truegraphics::widgets::Widget::set_size)
      .def("set_position", &truegraphics::widgets::Widget::set_position)
      .def("set_focusable", &truegraphics::widgets::Widget::set_focusable)
      .def("animate",
           [](const std::shared_ptr<truegraphics::widgets::Widget>& self,
              const std::string& property,
              double to,
              double duration,
              const std::string& easing) {
             auto* app = truegraphics::core::current_app();
             if (!app) {
               throw std::runtime_error("animate() must be called while the app is running.");
             }
             const bool smooth = (easing != "linear");
             app->animate_widget(self, property, to, duration, smooth);
           },
           py::arg("property"),
           py::arg("to"),
           py::arg("duration") = 0.3,
           py::arg("easing") = "smoothstep")
      .def("set_background", [](truegraphics::widgets::Widget& self, py::object color) {
        self.style().background = parse_color(color);
        self.style().use_gradient = false;
      })
      .def("set_text_color",
           [](truegraphics::widgets::Widget& self, py::object color) { self.style().foreground = parse_color(color); })
      .def("set_border_color",
           [](truegraphics::widgets::Widget& self, py::object color) { self.style().border = parse_color(color); })
      .def("set_border_radius", [](truegraphics::widgets::Widget& self, int radius) { self.style().corner_radius = radius; })
      .def("set_border_width", [](truegraphics::widgets::Widget& self, int w) { self.style().border_width = w; })
      .def("set_padding", [](truegraphics::widgets::Widget& self, int p) { self.style().padding = p; })
      .def("set_margin", [](truegraphics::widgets::Widget& self, int m) { self.style().margin = m; })
      .def("set_font", [](truegraphics::widgets::Widget& self, const std::string& family) { self.style().font_family = family; })
      .def("set_font_size", [](truegraphics::widgets::Widget& self, int size) { self.style().font_size = size; })
      .def("on_hover", [](truegraphics::widgets::Widget& self, py::function cb) {
        self.set_on_hover([cb = std::move(cb)](bool hovered) {
          py::gil_scoped_acquire gil;
          cb(hovered);
        });
      })
      .def("on_mouse_move", [](truegraphics::widgets::Widget& self, py::function cb) {
        self.set_on_mouse_move([cb = std::move(cb)](int32_t x, int32_t y) {
          py::gil_scoped_acquire gil;
          cb(x, y);
        });
      })
      .def("on_mouse_down", [](truegraphics::widgets::Widget& self, py::function cb) {
        self.set_on_mouse_down([cb = std::move(cb)](int32_t x, int32_t y) {
          py::gil_scoped_acquire gil;
          cb(x, y);
        });
      })
      .def("on_mouse_up", [](truegraphics::widgets::Widget& self, py::function cb) {
        self.set_on_mouse_up([cb = std::move(cb)](int32_t x, int32_t y) {
          py::gil_scoped_acquire gil;
          cb(x, y);
        });
      })
      .def("on_key_down", [](truegraphics::widgets::Widget& self, py::function cb) {
        self.set_on_key_down([cb = std::move(cb)](int32_t key) {
          py::gil_scoped_acquire gil;
          cb(key);
        });
      })
      .def("on_char", [](truegraphics::widgets::Widget& self, py::function cb) {
        self.set_on_char([cb = std::move(cb)](uint32_t codepoint) {
          py::gil_scoped_acquire gil;
          cb(codepoint);
        });
      })
      .def("on_drop_files", [](truegraphics::widgets::Widget& self, py::function cb) {
        self.set_on_drop_files([cb = std::move(cb)](std::vector<std::string> files) {
          py::gil_scoped_acquire gil;
          cb(std::move(files));
        });
      })
      .def_property_readonly("text", &truegraphics::widgets::Widget::text)
      .def_property_readonly("id", &truegraphics::widgets::Widget::id)
      .def_property_readonly("x", &truegraphics::widgets::Widget::x)
      .def_property_readonly("y", &truegraphics::widgets::Widget::y)
      .def_property_readonly("width", &truegraphics::widgets::Widget::width)
      .def_property_readonly("height", &truegraphics::widgets::Widget::height);

  py::class_<truegraphics::widgets::Button, truegraphics::widgets::Widget, std::shared_ptr<truegraphics::widgets::Button>>(m, "Button")
      .def(py::init([](const std::string& text, py::object on_click) {
             auto widget = create_and_attach<truegraphics::widgets::Button>(text);
             if (!on_click.is_none()) {
               py::function cb = on_click.cast<py::function>();
               widget->set_on_click([cb]() {
                 py::gil_scoped_acquire gil;
                 cb();
               });
             }
             return widget;
           }),
           py::arg("text"), py::arg("on_click") = py::none());

  py::class_<truegraphics::widgets::Label, truegraphics::widgets::Widget, std::shared_ptr<truegraphics::widgets::Label>>(m, "Label")
      .def(py::init([](const std::string& text) { return create_and_attach<truegraphics::widgets::Label>(text); }),
           py::arg("text") = "");

  py::class_<truegraphics::widgets::TextBox, truegraphics::widgets::Widget, std::shared_ptr<truegraphics::widgets::TextBox>>(m, "TextBox")
      .def(py::init([](const std::string& text) { return create_and_attach<truegraphics::widgets::TextBox>(text); }),
           py::arg("text") = "");

  py::class_<truegraphics::widgets::Checkbox, truegraphics::widgets::Widget, std::shared_ptr<truegraphics::widgets::Checkbox>>(m, "Checkbox")
      .def(py::init([](const std::string& text, bool checked, py::object on_change) {
             auto widget = create_and_attach<truegraphics::widgets::Checkbox>(text);
             widget->set_checked(checked);

             if (!on_change.is_none()) {
               py::function cb = on_change.cast<py::function>();
               widget->set_on_change([cb = std::move(cb)](bool value) {
                 py::gil_scoped_acquire gil;
                 cb(value);
               });
             }

             std::weak_ptr<truegraphics::widgets::Checkbox> weak = widget;
             widget->set_on_click([weak]() {
               if (auto w = weak.lock()) {
                 w->set_checked(!w->checked());
               }
             });

              return widget;
            }),
            py::arg("text") = "", py::arg("checked") = false, py::arg("on_change") = py::none());

  py::class_<truegraphics::widgets::RadioButton, truegraphics::widgets::Widget, std::shared_ptr<truegraphics::widgets::RadioButton>>(m, "RadioButton")
      .def(py::init([](const std::string& text, const std::string& group, bool checked, py::object on_change) {
             auto widget = create_and_attach<truegraphics::widgets::RadioButton>(text, group, checked);
             if (!on_change.is_none()) {
               py::function cb = on_change.cast<py::function>();
               widget->set_on_change([cb = std::move(cb)](bool value) {
                 py::gil_scoped_acquire gil;
                 cb(value);
               });
             }
             return widget;
           }),
           py::arg("text") = "",
           py::arg("group") = "default",
           py::arg("checked") = false,
           py::arg("on_change") = py::none())
      .def_property("checked", &truegraphics::widgets::RadioButton::checked, &truegraphics::widgets::RadioButton::set_checked)
      .def_property("group", &truegraphics::widgets::RadioButton::group, &truegraphics::widgets::RadioButton::set_group);

  py::class_<truegraphics::widgets::Slider, truegraphics::widgets::Widget, std::shared_ptr<truegraphics::widgets::Slider>>(m, "Slider")
      .def(py::init([](double min, double max, double value, py::object on_change) {
             auto widget = create_and_attach<truegraphics::widgets::Slider>(min, max, value);
             if (!on_change.is_none()) {
               py::function cb = on_change.cast<py::function>();
               widget->set_on_change([cb = std::move(cb)](double v) {
                 py::gil_scoped_acquire gil;
                 cb(v);
               });
             }
             return widget;
           }),
           py::arg("min") = 0.0,
           py::arg("max") = 1.0,
           py::arg("value") = 0.0,
           py::arg("on_change") = py::none())
      .def_property("value", &truegraphics::widgets::Slider::value, &truegraphics::widgets::Slider::set_value);

  py::class_<truegraphics::widgets::ProgressBar, truegraphics::widgets::Widget, std::shared_ptr<truegraphics::widgets::ProgressBar>>(m, "ProgressBar")
      .def(py::init([](double value) { return create_and_attach<truegraphics::widgets::ProgressBar>(value); }), py::arg("value") = 0.0)
      .def_property("value", &truegraphics::widgets::ProgressBar::value, &truegraphics::widgets::ProgressBar::set_value);

  py::class_<truegraphics::widgets::Dropdown, truegraphics::widgets::Widget, std::shared_ptr<truegraphics::widgets::Dropdown>>(m, "Dropdown")
      .def(py::init([](std::vector<std::string> items, int selected, py::object on_change) {
             auto widget = create_and_attach<truegraphics::widgets::Dropdown>(std::move(items), selected);
             if (!on_change.is_none()) {
               py::function cb = on_change.cast<py::function>();
               widget->set_on_change([cb = std::move(cb)](int idx) {
                 py::gil_scoped_acquire gil;
                 cb(idx);
               });
             }
             return widget;
           }),
           py::arg("items") = std::vector<std::string>{},
           py::arg("selected") = 0,
           py::arg("on_change") = py::none())
      .def("set_items", &truegraphics::widgets::Dropdown::set_items)
      .def_property("selected_index", &truegraphics::widgets::Dropdown::selected_index, &truegraphics::widgets::Dropdown::set_selected)
      .def_property_readonly("selected_text", &truegraphics::widgets::Dropdown::selected_text);

  py::class_<truegraphics::widgets::Tabs, truegraphics::widgets::Widget, std::shared_ptr<truegraphics::widgets::Tabs>>(m, "Tabs")
      .def(py::init([]() { return create_and_attach<truegraphics::widgets::Tabs>(); }))
      .def("tab", &truegraphics::widgets::Tabs::tab)
      .def_property("active_index", &truegraphics::widgets::Tabs::active_index, &truegraphics::widgets::Tabs::set_active_index);

  py::class_<truegraphics::widgets::TextArea, truegraphics::widgets::Widget, std::shared_ptr<truegraphics::widgets::TextArea>>(m, "TextArea")
      .def(py::init([](const std::string& text) { return create_and_attach<truegraphics::widgets::TextArea>(text); }), py::arg("text") = "");

  py::class_<truegraphics::widgets::PasswordBox, truegraphics::widgets::Widget, std::shared_ptr<truegraphics::widgets::PasswordBox>>(m, "PasswordBox")
      .def(py::init([](const std::string& text) { return create_and_attach<truegraphics::widgets::PasswordBox>(text); }), py::arg("text") = "");

  py::class_<truegraphics::widgets::ListView, truegraphics::widgets::Widget, std::shared_ptr<truegraphics::widgets::ListView>>(m, "ListView")
      .def(py::init([](std::vector<std::string> items, py::object on_select) {
             auto widget = create_and_attach<truegraphics::widgets::ListView>(std::move(items));
             if (!on_select.is_none()) {
               py::function cb = on_select.cast<py::function>();
               widget->set_on_select([cb = std::move(cb)](int idx) {
                 py::gil_scoped_acquire gil;
                 cb(idx);
               });
             }
             return widget;
           }),
           py::arg("items") = std::vector<std::string>{},
           py::arg("on_select") = py::none())
      .def("set_items", &truegraphics::widgets::ListView::set_items)
      .def_property("selected_index", &truegraphics::widgets::ListView::selected_index, &truegraphics::widgets::ListView::set_selected);

  py::class_<truegraphics::widgets::Table, truegraphics::widgets::Widget, std::shared_ptr<truegraphics::widgets::Table>>(m, "Table")
      .def(py::init([](std::vector<std::string> headers, std::vector<std::vector<std::string>> rows) {
             return create_and_attach<truegraphics::widgets::Table>(std::move(headers), std::move(rows));
           }),
           py::arg("headers") = std::vector<std::string>{},
           py::arg("rows") = std::vector<std::vector<std::string>>{})
      .def("set_headers", &truegraphics::widgets::Table::set_headers)
      .def("set_rows", &truegraphics::widgets::Table::set_rows);

  py::class_<truegraphics::widgets::TreeView, truegraphics::widgets::Widget, std::shared_ptr<truegraphics::widgets::TreeView>>(m, "TreeView")
      .def(py::init([](std::vector<std::pair<std::string, int>> items, py::object on_select) {
             std::vector<truegraphics::widgets::TreeView::Item> converted;
             converted.reserve(items.size());
             for (auto& [text, level] : items) {
               converted.push_back(truegraphics::widgets::TreeView::Item{std::move(text), level});
             }

             auto widget = create_and_attach<truegraphics::widgets::TreeView>(std::move(converted));
             if (!on_select.is_none()) {
               py::function cb = on_select.cast<py::function>();
               widget->set_on_select([cb = std::move(cb)](int idx) {
                 py::gil_scoped_acquire gil;
                 cb(idx);
               });
             }
             return widget;
           }),
           py::arg("items") = std::vector<std::pair<std::string, int>>{},
           py::arg("on_select") = py::none())
      .def("set_items", [](truegraphics::widgets::TreeView& self, std::vector<std::pair<std::string, int>> items) {
        std::vector<truegraphics::widgets::TreeView::Item> converted;
        converted.reserve(items.size());
        for (auto& [text, level] : items) {
          converted.push_back(truegraphics::widgets::TreeView::Item{std::move(text), level});
        }
        self.set_items(std::move(converted));
      })
      .def_property("selected_index", &truegraphics::widgets::TreeView::selected_index, &truegraphics::widgets::TreeView::set_selected);

  py::class_<truegraphics::widgets::Image, truegraphics::widgets::Widget, std::shared_ptr<truegraphics::widgets::Image>>(m, "Image")
      .def(py::init([](const std::string& path) { return create_and_attach<truegraphics::widgets::Image>(path); }),
           py::arg("path") = "")
      .def("set_source", &truegraphics::widgets::Image::set_source)
      .def_property_readonly("source", &truegraphics::widgets::Image::source);

  py::class_<truegraphics::widgets::ScrollView, truegraphics::widgets::Widget, std::shared_ptr<truegraphics::widgets::ScrollView>>(m, "ScrollView")
      .def(py::init([]() { return create_and_attach<truegraphics::widgets::ScrollView>(); }))
      .def("set_scroll_x", &truegraphics::widgets::ScrollView::set_scroll_x)
      .def_property_readonly("scroll_x", &truegraphics::widgets::ScrollView::scroll_x)
      .def("scroll_by_x", &truegraphics::widgets::ScrollView::scroll_by_x)
      .def("set_scroll_y", &truegraphics::widgets::ScrollView::set_scroll_y)
      .def_property_readonly("scroll_y", &truegraphics::widgets::ScrollView::scroll_y)
      .def("scroll_by", &truegraphics::widgets::ScrollView::scroll_by)
      .def("__enter__", [](const std::shared_ptr<truegraphics::widgets::ScrollView>& self) {
        push_parent(self);
        return self;
      })
      .def("__exit__",
           [](const std::shared_ptr<truegraphics::widgets::ScrollView>& self,
              py::object /*exc_type*/,
              py::object /*exc*/,
              py::object /*tb*/) {
             pop_parent(self);
             return false;
           });

  py::class_<truegraphics::widgets::Container, truegraphics::widgets::Widget, std::shared_ptr<truegraphics::widgets::Container>>(m, "Container")
      .def(py::init([]() { return create_and_attach<truegraphics::widgets::Container>(); }))
      .def("set_layout", &truegraphics::widgets::Container::set_layout)
      .def("set_align", &truegraphics::widgets::Container::set_align)
      .def("set_justify", &truegraphics::widgets::Container::set_justify)
      .def("set_gap", &truegraphics::widgets::Container::set_gap)
      .def("set_grid_columns", &truegraphics::widgets::Container::set_grid_columns)
      .def("__enter__", [](const std::shared_ptr<truegraphics::widgets::Container>& self) {
        push_parent(self);
        return self;
      })
      .def("__exit__",
           [](const std::shared_ptr<truegraphics::widgets::Container>& self,
              py::object /*exc_type*/,
              py::object /*exc*/,
              py::object /*tb*/) {
             pop_parent(self);
             return false;
           });

  m.def("Row", []() {
    auto c = create_and_attach<truegraphics::widgets::Container>();
    c->set_layout(truegraphics::widgets::Container::LayoutMode::Row);
    return c;
  });

  m.def("Column", []() {
    auto c = create_and_attach<truegraphics::widgets::Container>();
    c->set_layout(truegraphics::widgets::Container::LayoutMode::Column);
    return c;
  });

  m.def("Grid", [](int columns) {
    auto c = create_and_attach<truegraphics::widgets::Container>();
    c->set_layout(truegraphics::widgets::Container::LayoutMode::Grid);
    c->set_grid_columns(columns);
    return c;
  });

  m.def("Absolute", []() {
    auto c = create_and_attach<truegraphics::widgets::Container>();
    c->set_layout(truegraphics::widgets::Container::LayoutMode::None);
    c->style().padding = 0;
    return c;
  });

  py::class_<truegraphics::widgets::Canvas, truegraphics::widgets::Widget, std::shared_ptr<truegraphics::widgets::Canvas>>(m, "Canvas")
      .def(py::init([]() { return create_and_attach<truegraphics::widgets::Canvas>(); }))
      .def("clear", &truegraphics::widgets::Canvas::clear_commands)
      .def("draw_line",
           [](truegraphics::widgets::Canvas& self, int x1, int y1, int x2, int y2, py::object color, int thickness) {
             self.draw_line(x1, y1, x2, y2, parse_color(color), thickness);
           },
           py::arg("x1"), py::arg("y1"), py::arg("x2"), py::arg("y2"), py::arg("color") = "#FFFFFF",
           py::arg("thickness") = 1)
      .def("draw_rect",
           [](truegraphics::widgets::Canvas& self,
              int x,
              int y,
              int w,
              int h,
              py::object color,
              int radius,
              bool filled,
              int thickness) {
             self.draw_rect_cmd(x, y, w, h, parse_color(color), radius, filled, thickness);
           },
           py::arg("x"), py::arg("y"), py::arg("w"), py::arg("h"), py::arg("color") = "#FFFFFF", py::arg("radius") = 0,
           py::arg("filled") = true, py::arg("thickness") = 1)
      .def("draw_circle",
           [](truegraphics::widgets::Canvas& self,
              int cx,
              int cy,
              int radius,
              py::object color,
              bool filled,
              int thickness) { self.draw_circle(cx, cy, radius, parse_color(color), filled, thickness); },
           py::arg("cx"), py::arg("cy"), py::arg("radius"), py::arg("color") = "#FFFFFF", py::arg("filled") = true,
           py::arg("thickness") = 1)
      .def("draw_text",
           [](truegraphics::widgets::Canvas& self, int x, int y, const std::string& text, py::object color) {
             self.draw_text_cmd(x, y, text, parse_color(color));
           },
           py::arg("x"), py::arg("y"), py::arg("text"), py::arg("color") = "#FFFFFF");

  py::class_<truegraphics::state::StateInt>(m, "StateInt")
      .def(py::init<int>(), py::arg("value") = 0)
      .def("get", &truegraphics::state::StateInt::get)
      .def("set", &truegraphics::state::StateInt::set)
      .def("subscribe", [](truegraphics::state::StateInt& self, py::function cb) {
        self.subscribe([cb = std::move(cb)](int value) {
          py::gil_scoped_acquire gil;
          cb(value);
        });
      });

  py::class_<truegraphics::state::StateFloat>(m, "StateFloat")
      .def(py::init<double>(), py::arg("value") = 0.0)
      .def("get", &truegraphics::state::StateFloat::get)
      .def("set", &truegraphics::state::StateFloat::set)
      .def("subscribe", [](truegraphics::state::StateFloat& self, py::function cb) {
        self.subscribe([cb = std::move(cb)](double value) {
          py::gil_scoped_acquire gil;
          cb(value);
        });
      });

  py::class_<truegraphics::state::StateBool>(m, "StateBool")
      .def(py::init<bool>(), py::arg("value") = false)
      .def("get", &truegraphics::state::StateBool::get)
      .def("set", &truegraphics::state::StateBool::set)
      .def("subscribe", [](truegraphics::state::StateBool& self, py::function cb) {
        self.subscribe([cb = std::move(cb)](bool value) {
          py::gil_scoped_acquire gil;
          cb(value);
        });
      });

  py::class_<truegraphics::state::StateString>(m, "StateString")
      .def(py::init<std::string>(), py::arg("value") = "")
      .def("get", &truegraphics::state::StateString::get, py::return_value_policy::reference_internal)
      .def("set", &truegraphics::state::StateString::set)
      .def("subscribe", [](truegraphics::state::StateString& self, py::function cb) {
        self.subscribe([cb = std::move(cb)](const std::string& value) {
          py::gil_scoped_acquire gil;
          cb(value);
        });
      });
}
