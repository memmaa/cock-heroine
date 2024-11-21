
if (UNIX AND NOT APPLE)
	install(FILES CockHeroine.desktop DESTINATION share/applications)
	install(FILES WandIcon.png DESTINATION share/icons/hicolor/150x150/apps)
endif()
