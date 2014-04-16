AM_CXXFLAGS = -I$(top_srcdir)/source/ -I$(top_srcdir)/source/Lua/src

bin_PROGRAMS = epiar

epiar_SOURCES = source/main.cpp \
		source/menu.cpp \
                source/Audio/audio.cpp \
                source/Audio/audio_lua.cpp \
                source/Audio/music.cpp \
                source/Audio/sound.cpp \
                source/Engine/alliances.cpp \
                source/Engine/commodities.cpp \
                source/Engine/console.cpp \
                source/Engine/calendar.cpp \
                source/Engine/camera.cpp \
                source/Engine/engines.cpp \
                source/Engine/hud.cpp \
                source/Engine/models.cpp \
                source/Engine/mission.cpp \
                source/Engine/outfit.cpp \
                source/Engine/simulation.cpp \
                source/Engine/simulation_lua.cpp \
                source/Engine/starfield.cpp \
                source/Engine/technologies.cpp \
                source/Engine/weapons.cpp \
                source/Graphics/animation.cpp \
                source/Graphics/font.cpp \
                source/Graphics/image.cpp \
                source/Graphics/video.cpp \
                source/Input/input.cpp \
                source/Sprites/ai.cpp \
                source/Sprites/ai_lua.cpp \
                source/Sprites/effects.cpp \
                source/Sprites/gate.cpp \
                source/Sprites/planets.cpp \
                source/Sprites/planets_lua.cpp \
                source/Sprites/player.cpp \
                source/Sprites/projectile.cpp \
                source/Sprites/ship.cpp \
                source/Sprites/sprite.cpp \
                source/Sprites/spritemanager.cpp \
                source/UI/ui.cpp \
                source/UI/ui_action.cpp \
                source/UI/ui_button.cpp \
                source/UI/ui_checkbox.cpp \
                source/UI/ui_container.cpp \
                source/UI/ui_dropdown.cpp \
                source/UI/ui_label.cpp \
                source/UI/ui_lua.cpp \
                source/UI/ui_map.cpp \
                source/UI/ui_picture.cpp \
                source/UI/ui_paragraph.cpp \
                source/UI/ui_scrollbar.cpp \
                source/UI/ui_slider.cpp \
                source/UI/ui_tabs.cpp \
                source/UI/ui_text.cpp \
                source/UI/ui_textarea.cpp \
                source/UI/ui_textbox.cpp \
                source/UI/ui_widget.cpp \
                source/UI/ui_window.cpp \
                source/UI/ui_frame.cpp \
		source/UI/ui_dialogs.cpp \
                source/Utilities/argparser.cpp \
                source/Utilities/components.cpp \
                source/Utilities/coordinate.cpp \
                source/Utilities/file.cpp \
                source/Utilities/filesystem.cpp \
                source/Utilities/log.cpp \
                source/Utilities/lua.cpp \
                source/Utilities/options.cpp \
                source/Utilities/quadtree.cpp \
                source/Utilities/resource.cpp \
                source/Utilities/timer.cpp \
                source/Utilities/trig.cpp \
                source/Utilities/xml.cpp

epiar_LDADD = source/Lua/src/liblua.a

SUBDIRS=source/Lua

include Resources/Animations/Makefile.am
include Resources/Audio/Makefile.am
include Resources/Definitions/Makefile.am
include Resources/Graphics/Makefile.am
include Resources/Scripts/Makefile.am
include Resources/Skin/Makefile.am
include Resources/Art/Makefile.am
include Resources/Fonts/Makefile.am
include Resources/Icons/Makefile.am
include Resources/Simulation/Makefile.am

AM_CPPFLAGS = -DDATADIR="\"$(datadir)/epiar\""

