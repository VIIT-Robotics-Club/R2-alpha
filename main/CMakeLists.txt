idf_component_register(SRCS "mpuPub.cpp" "clawServer.cpp" "velDriver.cpp" "lunaPub.cpp" "main.cpp" "icmPub.cpp"
                    INCLUDE_DIRS "."
                    REQUIRES urosHandler icm-20948 qmd luna lsaHandler MPUdriver)
