set(UIDEMO_EN_RESOURCES
    ../DemoAssets/Resources/en/image.png
    ../DemoAssets/Resources/en/animation.json)
set(UIDEMO_DE_RESOURCES
    ../DemoAssets/Resources/de/image@2x.png
    ../DemoAssets/Resources/de/image.png
    ../DemoAssets/Resources/de/animation.json)


ios_configure_resources(LANGUAGE de PATH images FILES ${UIDEMO_DE_RESOURCES})
ios_configure_resources(LANGUAGE en PATH images FILES ${UIDEMO_EN_RESOURCES})


set(UIDEMO_RESOURCES ${UIDEMO_EN_RESOURCES} ${UIDEMO_DE_RESOURCES})
