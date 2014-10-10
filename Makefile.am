AM_CXXFLAGS = -I$(top_srcdir)/source/ -I$(top_srcdir)/source/lua/src

bin_PROGRAMS = epiar

epiar_SOURCES = source/main.cpp \
		source/menu.cpp \
                source/audio/audio.cpp \
                source/audio/audio_lua.cpp \
                source/audio/music.cpp \
                source/audio/sound.cpp \
                source/engine/alliances.cpp \
                source/engine/commodities.cpp \
                source/engine/console.cpp \
                source/engine/calendar.cpp \
                source/engine/camera.cpp \
                source/engine/engines.cpp \
                source/engine/hud.cpp \
                source/engine/models.cpp \
                source/engine/mission.cpp \
                source/engine/outfit.cpp \
                source/engine/simulation.cpp \
                source/engine/simulation_lua.cpp \
                source/engine/starfield.cpp \
                source/engine/technologies.cpp \
                source/engine/weapons.cpp \
                source/graphics/animation.cpp \
                source/graphics/font.cpp \
                source/graphics/image.cpp \
                source/graphics/video.cpp \
                source/input/input.cpp \
                source/sprites/ai.cpp \
                source/sprites/ai_lua.cpp \
                source/sprites/effects.cpp \
                source/sprites/gate.cpp \
                source/sprites/planets.cpp \
                source/sprites/planets_lua.cpp \
                source/sprites/player.cpp \
                source/sprites/projectile.cpp \
                source/sprites/ship.cpp \
                source/sprites/sprite.cpp \
                source/sprites/spritemanager.cpp \
                source/ui/ui.cpp \
                source/ui/ui_action.cpp \
                source/ui/ui_button.cpp \
                source/ui/ui_checkbox.cpp \
                source/ui/ui_container.cpp \
                source/ui/ui_dropdown.cpp \
                source/ui/ui_label.cpp \
                source/ui/ui_lua.cpp \
                source/ui/ui_map.cpp \
                source/ui/ui_picture.cpp \
                source/ui/ui_paragraph.cpp \
                source/ui/ui_scrollbar.cpp \
                source/ui/ui_slider.cpp \
                source/ui/ui_tabs.cpp \
                source/ui/ui_text.cpp \
                source/ui/ui_textarea.cpp \
                source/ui/ui_textbox.cpp \
                source/ui/ui_widget.cpp \
                source/ui/ui_window.cpp \
                source/ui/ui_frame.cpp \
                source/ui/ui_dialogs.cpp \
                source/utilities/argparser.cpp \
                source/utilities/components.cpp \
                source/utilities/coordinate.cpp \
                source/utilities/file.cpp \
                source/utilities/filesystem.cpp \
                source/utilities/log.cpp \
                source/utilities/lua.cpp \
                source/utilities/options.cpp \
                source/utilities/quadtree.cpp \
                source/utilities/resource.cpp \
                source/utilities/timer.cpp \
                source/utilities/trig.cpp \
                source/utilities/xml.cpp

epiar_LDADD = source/lua/src/liblua.a

SUBDIRS=source/lua

include resources/animations/Makefile.am
include resources/audio/Makefile.am
include resources/definitions/Makefile.am
include resources/graphics/Makefile.am
include resources/scripts/Makefile.am
include resources/skin/Makefile.am
include resources/art/Makefile.am
include resources/fonts/Makefile.am
include resources/icons/Makefile.am
include resources/simulation/Makefile.am

AM_CPPFLAGS = -DDATADIR="\"$(datadir)/epiar\""

