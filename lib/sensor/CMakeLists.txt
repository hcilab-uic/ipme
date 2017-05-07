set(SRCS walabot.cpp
    # Adding these headers so that QtCreator IDE can show them in projects view
    # I know this is dumb, but there is no other way to do it
    receiver.h
    sensor.h
    )

add_library(sensor ${SRCS})
