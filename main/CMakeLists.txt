idf_component_register(SRCS "resetClient.cpp" "lf_pub.cpp" "mpuPub.cpp" "clawServer.cpp" "velDriver.cpp" "lunaPub.cpp" "main.cpp" "icmPub.cpp"
                    INCLUDE_DIRS "."
                    REQUIRES urosHandler icm-20948 qmd luna lsaHandler MPUdriver)
