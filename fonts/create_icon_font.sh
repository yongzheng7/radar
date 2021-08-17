#!/bin/sh

# This script creates a subset of source icon TTF font with only needed icon glyphs included.
# Install fonttools via your package manager or pip3 package manager
SOURCE_FONT=materialdesignicons-webfont.ttf
REDUCED_FONT=icons.ttf
INCLUDED_GLYPHS_LIST_FILE=used_glyphs.txt

pyftsubset --output-file="${REDUCED_FONT}" --unicodes-file="${INCLUDED_GLYPHS_LIST_FILE}" "${SOURCE_FONT}"
