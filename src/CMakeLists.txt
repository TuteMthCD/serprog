add_executable(${ProjectName}
    main.cpp
    usb.cpp
    programmer.cpp
)

target_include_directories(${ProjectName} PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}
)

target_link_libraries(${ProjectName} 
    pico_stdlib 
    FreeRTOS-Kernel-Heap4 
    pico_cyw43_arch_none
    hardware_spi
    )

pico_add_extra_outputs(${ProjectName})
