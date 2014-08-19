AM_CXXFLAGS = -I$(top_srcdir)/src/ -I$(top_srcdir)/src/lua/src

bin_PROGRAMS = epiar

epiar_SOURCES = src/main.cpp \
                src/menu.cpp \
                src/audio/audio.cpp \
                src/audio/audio_lua.cpp \
                src/audio/music.cpp \
                src/audio/sound.cpp \
                src/engine/alliances.cpp \
                src/engine/commodities.cpp \
                src/engine/console.cpp \
                src/engine/calendar.cpp \
                src/engine/camera.cpp \
                src/engine/engines.cpp \
                src/engine/hud.cpp \
                src/engine/map.cpp \
                src/engine/models.cpp \
                src/engine/mission.cpp \
                src/engine/outfit.cpp \
                src/engine/scenario.cpp \
                src/engine/scenario_lua.cpp \
                src/engine/sector.cpp \
                src/engine/simulation.cpp \
                src/engine/simulation_lua.cpp \
                src/engine/starfield.cpp \
                src/engine/technologies.cpp \
                src/engine/weapons.cpp \
                src/graphics/animation.cpp \
                src/graphics/font.cpp \
                src/graphics/image.cpp \
                src/graphics/video.cpp \
                src/input/input.cpp \
                src/sprites/ai.cpp \
                src/sprites/ai_lua.cpp \
                src/sprites/effects.cpp \
                src/sprites/planets.cpp \
                src/sprites/planets_lua.cpp \
                src/sprites/player.cpp \
                src/sprites/projectile.cpp \
                src/sprites/ship.cpp \
                src/sprites/sprite.cpp \
                src/sprites/spritemanager.cpp \
                src/ui/ui.cpp \
                src/ui/ui_action.cpp \
                src/ui/ui_button.cpp \
                src/ui/ui_checkbox.cpp \
                src/ui/ui_container.cpp \
                src/ui/ui_dropdown.cpp \
                src/ui/ui_label.cpp \
                src/ui/ui_lua.cpp \
                src/ui/ui_navmap.cpp \
                src/ui/ui_picture.cpp \
                src/ui/ui_paragraph.cpp \
                src/ui/ui_scrollbar.cpp \
                src/ui/ui_slider.cpp \
                src/ui/ui_tabs.cpp \
                src/ui/ui_text.cpp \
                src/ui/ui_textarea.cpp \
                src/ui/ui_textbox.cpp \
                src/ui/ui_widget.cpp \
                src/ui/ui_window.cpp \
                src/ui/ui_frame.cpp \
                src/ui/ui_dialogs.cpp \
                src/utilities/argparser.cpp \
                src/utilities/components.cpp \
                src/utilities/coordinate.cpp \
                src/utilities/file.cpp \
                src/utilities/filesystem.cpp \
                src/utilities/log.cpp \
                src/utilities/lua.cpp \
                src/utilities/options.cpp \
                src/utilities/quadtree.cpp \
                src/utilities/resource.cpp \
                src/utilities/timer.cpp \
                src/utilities/trig.cpp \
                src/utilities/xmlfile.cpp

epiar_LDADD = src/lua/src/liblua.a

SUBDIRS=src/lua

include resources/animations/Makefile.am
include resources/audio/Makefile.am
include resources/definitions/Makefile.am
include resources/graphics/Makefile.am
include resources/scripts/Makefile.am
include resources/skin/Makefile.am
include resources/art/Makefile.am
include resources/fonts/Makefile.am
include resources/icons/Makefile.am
include resources/scenarios/Makefile.am

AM_CPPFLAGS = -DDATADIR="\"$(datadir)/epiar\""

