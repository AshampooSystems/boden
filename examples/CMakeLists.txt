add_subdirectory(BodenDemo)
add_subdirectory(reddit)
add_subdirectory(KeeWeb)
add_subdirectory(KeyboardTest)

set_property(TARGET bodendemo PROPERTY FOLDER "Boden/Examples")
set_property(TARGET reddit PROPERTY FOLDER "Boden/Examples")
set_property(TARGET keeweb PROPERTY FOLDER "Boden/Examples")
set_property(TARGET KeyboardTest PROPERTY FOLDER "Boden/Examples")

