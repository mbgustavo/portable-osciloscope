# Configuration for cmake-format (cmakelang).
# See: https://cmake-format.readthedocs.io/en/latest/configuration.html

with section("format"):
    line_width = 120
    tab_size = 2
    use_tabchars = False
    max_pargs_hwrap = 2

with section("lint"):
    disabled_codes = ["C0301"]
