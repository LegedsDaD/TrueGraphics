#pragma once

#include <string>
#include <vector>

namespace truegraphics::resource {

void add_path(const std::string& path);
void clear_paths();
std::vector<std::string> paths();

// Resolves `path` against registered resource paths.
// If `path` already exists on disk, it is returned unchanged.
// If no match is found, the original input is returned.
std::string resolve(const std::string& path);

// Loads a font file for this process (Windows FR_PRIVATE).
// Returns true if at least one font resource was added.
bool load_font(const std::string& path);
bool unload_font(const std::string& path);

}  // namespace truegraphics::resource

