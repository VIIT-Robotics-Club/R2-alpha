idf_component_register(SRCS "icmPub.cpp" "clawServer.cpp" "velDriver.cpp" "lunaPub.cpp" "main.cpp"
                    INCLUDE_DIRS "."
                    REQUIRES urosHandler icm-20948 qmd luna lsaHandler)
