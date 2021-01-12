include(Sanitize)
include(UseGold)
include(LinkerWarnings)
include(DebugMode)

function(score_cotire_pre TheTarget)
  if(SCORE_COTIRE)
    if(SCORE_COTIRE_DISABLE_UNITY)
      set_property(TARGET ${TheTarget} PROPERTY COTIRE_ADD_UNITY_BUILD FALSE)
    endif()

    if(SCORE_COTIRE_ALL_HEADERS)
      set_target_properties(${TheTarget} PROPERTIES COTIRE_PREFIX_HEADER_IGNORE_PATH "")
    endif()

    # FIXME on windows
    set_target_properties(${TheTarget} PROPERTIES
      COTIRE_PREFIX_HEADER_IGNORE_PATH "${COTIRE_PREFIX_HEADER_IGNORE_PATH};/usr/include/boost/preprocessor/")

    if(NOT ${TheTarget} STREQUAL "score_lib_base")
      # We reuse the same prefix header

      get_target_property(SCORE_COMMON_PREFIX_HEADER score_lib_base COTIRE_CXX_PREFIX_HEADER)
      set_target_properties(${TheTarget} PROPERTIES COTIRE_CXX_PREFIX_HEADER_INIT "${SCORE_COMMON_PREFIX_HEADER}")
    endif()

  endif()
endfunction()

function(score_cotire_post TheTarget)
endfunction()

### Call at the beginning of a plug-in cmakelists ###
function(score_common_setup)
  enable_testing()
  set(CMAKE_INCLUDE_CURRENT_DIR ON)
  set(CMAKE_POSITION_INDEPENDENT_CODE ON)
  set(CMAKE_AUTOUIC OFF)
  set(CMAKE_AUTOMOC OFF)
  cmake_policy(SET CMP0020 NEW)
  cmake_policy(SET CMP0042 NEW)
endfunction()


### Initialization of most common stuff ###

function(score_set_msvc_compile_options TheTarget)
    target_compile_options(${TheTarget} PUBLIC
#    "/Za"
    "-wd4180"
    "-wd4224"
    "-wd4068" # pragma mark -
    "-wd4250" # inherits via dominance
    "-wd4251" # DLL stuff
    "-wd4275" # DLL stuff
    "-wd4244" # return : conversion from foo to bar, possible loss of data
    "-wd4800" # conversion from int to bool, performance warning
    "-wd4503" # decorated name length exceeded
    "-MP"
    "-std:c++latest"
    )

    target_compile_definitions(${TheTarget} PUBLIC
        "NOMINMAX"
#        "__cpp_constexpr=201304"
#        "__cpp_variable_templates=201304"
        )
endfunction()

function(score_set_apple_compile_options TheTarget)
endfunction()

function(score_set_gcc_compile_options TheTarget)
    # set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror -Wno-error=shadow -Wno-error=switch -Wno-error=switch-enum -Wno-error=empty-body -Wno-error=overloaded-virtual -Wno-error=suggest-final-methods -Wno-error=suggest-final-types -Wno-error=suggest-override -Wno-error=maybe-uninitialized")
        target_compile_options(${TheTarget} PUBLIC
          -Wno-div-by-zero
          -Wsuggest-final-types
          -Wsuggest-final-methods
          -Wsuggest-override
          -Wpointer-arith
          -Wsuggest-attribute=noreturn
          -Wno-missing-braces
          -Wmissing-field-initializers
          -Wformat=2
          -Wno-format-nonliteral
          -Wpedantic
          -Werror=return-local-addr
          )

      if(NOT SCORE_SANITIZE)
      target_compile_options(${TheTarget} PUBLIC
          "$<$<CONFIG:Release>:-ffunction-sections>"
          "$<$<CONFIG:Release>:-fdata-sections>"
          "$<$<CONFIG:Release>:-Wl,--gc-sections>"
          "$<$<CONFIG:Debug>:-O0>"
          "$<$<CONFIG:Debug>:-ggdb>"
      )

    if(SCORE_SPLIT_DEBUG)
      target_link_libraries(${TheTarget} PUBLIC
                  "$<$<CONFIG:Debug>:-fvar-tracking-assignments>"
        )
    endif()

      get_target_property(NO_LTO ${TheTarget} SCORE_TARGET_NO_LTO)
      if(NOT ${NO_LTO})
          target_compile_options(${TheTarget} PUBLIC
          )
      endif()
      target_link_libraries(${TheTarget} PUBLIC
          "$<$<CONFIG:Release>:-ffunction-sections>"
          "$<$<CONFIG:Release>:-fdata-sections>"
          "$<$<CONFIG:Release>:-Wl,--gc-sections>"
          "$<$<CONFIG:Debug>:-fvar-tracking-assignments>"
          "$<$<CONFIG:Debug>:-O0>"
          "$<$<CONFIG:Debug>:-ggdb>"
          )

      endif()
      # -Wcast-qual is nice but requires more work...
      # -Wzero-as-null-pointer-constant  is garbage
      # Too much clutter :set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wswitch-enum -Wshadow  -Wsuggest-attribute=const  -Wsuggest-attribute=pure ")
endfunction()

function(score_set_clang_compile_options TheTarget)
    target_compile_options(${TheTarget} PUBLIC
        -Wno-gnu-string-literal-operator-template
        -Wno-missing-braces
        -Werror=return-stack-address
        -Wmissing-field-initializers
        -Wno-gnu-statement-expression
      #  -Wweak-vtables
        -ftemplate-backtrace-limit=0
        "$<$<CONFIG:Debug>:-O0>"
        )
    #if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    #	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Weverything -Wno-c++98-compat -Wno-exit-time-destructors -Wno-padded")
    #endif()
endfunction()

function(score_set_linux_compile_options TheTarget)
    use_gold(${TheTarget})

    if(NOT SCORE_SANITIZE AND LINKER_IS_GOLD AND SCORE_SPLIT_DEBUG)
        target_compile_options(${TheTarget} PUBLIC
            # Debug options
            "$<$<CONFIG:Debug>:-gsplit-dwarf>")
    endif()
    target_compile_options(${TheTarget} PUBLIC
        # Debug options
        "$<$<CONFIG:Debug>:-ggdb>"
        "$<$<CONFIG:Debug>:-O0>")
    target_link_libraries(${TheTarget} PUBLIC
        # Debug options
        "$<$<CONFIG:Debug>:-ggdb>"
        "$<$<CONFIG:Debug>:-O0>")
endfunction()

function(score_set_unix_compile_options TheTarget)
    # General options

    #set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wabi -Wctor-dtor-privacy -Wnon-virtual-dtor -Wreorder -Wstrict-null-sentinel -Wno-non-template-friend -Woverloaded-virtual -Wno-pmf-conversions -Wsign-promo -Wextra -Wall -Waddress -Waggregate-return -Warray-bounds -Wno-attributes -Wno-builtin-macro-redefined")
    #set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wc++0x-compat -Wcast-align -Wcast-qual -Wchar-subscripts -Wclobbered -Wcomment -Wconversion -Wcoverage-mismatch -Wno-deprecated -Wno-deprecated-declarations -Wdisabled-optimization -Wno-div-by-zero -Wempty-body -Wenum-compare")

    target_compile_options(${TheTarget} PUBLIC
    -Wall
    -Wextra
    -Wno-unused-parameter
    -Wno-unknown-pragmas
    -Wno-four-char-constants
    -Wnon-virtual-dtor
    -pedantic
    -Woverloaded-virtual
    -pipe
    -Wno-missing-declarations
    -Werror=redundant-decls
    -Werror=return-type
    -Werror=trigraphs
    # -std=c++17
    # Release options
    "$<$<AND:$<CONFIG:Release>,$<BOOL:${NACL}>>:-O3>"
    "$<$<AND:$<CONFIG:Release>,$<NOT:$<BOOL:${NACL}>>>:-Ofast>"
    "$<$<AND:$<CONFIG:Release>,$<NOT:$<BOOL:${NACL}>>>:-fno-finite-math-only>"
    "$<$<AND:$<CONFIG:Release>,$<BOOL:${SCORE_ENABLE_OPTIMIZE_CUSTOM}>>:-march=native>"
    )

    target_link_libraries(${TheTarget} PUBLIC
        "$<$<CONFIG:Release>:-Ofast>"
        "$<$<CONFIG:Release>:-fno-finite-math-only>"
        "$<$<AND:$<CONFIG:Release>,$<BOOL:${SCORE_ENABLE_OPTIMIZE_CUSTOM}>>:-march=native>")
endfunction()

function(score_set_compile_options TheTarget)
  #set_target_properties(${TheTarget} PROPERTIES CXX_STANDARD 17)
  target_compile_definitions(${TheTarget} PUBLIC
      $<$<CONFIG:Debug>:SCORE_DEBUG>
      $<$<CONFIG:Debug>:BOOST_MULTI_INDEX_ENABLE_INVARIANT_CHECKING>
      $<$<CONFIG:Debug>:BOOST_MULTI_INDEX_ENABLE_SAFE_MODE>

# various options

      $<$<BOOL:${SCORE_IEEE}>:SCORE_IEEE_SKIN>
      $<$<BOOL:${SCORE_WEBSOCKETS}>:SCORE_WEBSOCKETS>
      $<$<BOOL:${SCORE_OPENGL}>:SCORE_OPENGL>
      $<$<BOOL:${DEPLOYMENT_BUILD}>:SCORE_DEPLOYMENT_BUILD>
      $<$<BOOL:${SCORE_STATIC_PLUGINS}>:SCORE_STATIC_PLUGINS>
      )
  get_target_property(theType ${TheTarget} TYPE)

  if(${theType} MATCHES STATIC_LIBRARY)
    target_compile_definitions(${TheTarget} PRIVATE
      $<$<BOOL:${SCORE_STATIC_PLUGINS}>:QT_STATICPLUGIN>
    )
  endif()

  if(SCORE_SANITIZE)
      get_target_property(NO_SANITIZE ${TheTarget} SCORE_TARGET_NO_SANITIZE)
      if(NOT "${NO_SANITIZE}")
          sanitize_build(${TheTarget})
      endif()
      # debugmode_build(${TheTarget})
  endif()

  if (CXX_IS_GCC OR CXX_IS_CLANG)
    score_set_unix_compile_options(${TheTarget})
  endif()

  if (CXX_IS_CLANG)
      score_set_clang_compile_options(${TheTarget})
  endif()

  if (CXX_IS_MSVC)
      score_set_msvc_compile_options(${TheTarget})
  endif()

  if(CXX_IS_GCC)
      score_set_gcc_compile_options(${TheTarget})
  endif()

  # OS X
  if(APPLE)
      score_set_apple_compile_options(${TheTarget})
  endif()

  # Linux
  if(NOT APPLE AND NOT WIN32)
      score_set_linux_compile_options(${TheTarget})
  endif()

  # currently breaks build : add_linker_warnings(${TheTarget})
endfunction()

function(setup_score_common_features TheTarget)
  score_set_compile_options(${TheTarget})
  score_cotire_pre(${TheTarget})

  if(ENABLE_LTO)
    set_property(TARGET ${TheTarget}
                 PROPERTY INTERPROCEDURAL_OPTIMIZATION True)
  endif()

  if(SCORE_STATIC_PLUGINS)
    target_compile_definitions(${TheTarget}
                               PUBLIC SCORE_STATIC_PLUGINS)
  endif()

  target_include_directories(${TheTarget} INTERFACE "${CMAKE_CURRENT_BINARY_DIR}")
endfunction()


### Initialization of common stuff ###
function(setup_score_common_exe_features TheTarget)
    setup_score_common_features("${TheTarget}")
  score_cotire_post("${TheTarget}")
endfunction()

function(setup_score_common_test_features TheTarget)
    setup_score_common_features("${TheTarget}")
endfunction()

function(setup_score_common_lib_features TheTarget)
  setup_score_common_features("${TheTarget}")

  if(NOT SCORE_STATIC_PLUGINS)
    set_target_properties(${TheTarget} PROPERTIES CXX_VISIBILITY_PRESET hidden)
    set_target_properties(${TheTarget} PROPERTIES VISIBILITY_INLINES_HIDDEN 1)
  endif()
  generate_export_header(${TheTarget})

  string(TOUPPER "${TheTarget}" UPPERCASE_PLUGIN_NAME)
  set_property(TARGET ${TheTarget} APPEND
               PROPERTY INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_SOURCE_DIR}")
  set_property(TARGET ${TheTarget} APPEND
               PROPERTY INTERFACE_COMPILE_DEFINITIONS "${UPPERCASE_PLUGIN_NAME}")
endfunction()


### Call with a library target ###
function(setup_score_library PluginName)
  setup_score_common_lib_features("${PluginName}")

  set(SCORE_LIBRARIES_LIST ${SCORE_LIBRARIES_LIST} "${PluginName}" CACHE INTERNAL "List of libraries")
  set_target_properties(${PluginName} PROPERTIES
      LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/plugins/"
      RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/plugins/")

  if(NOT SCORE_STATIC_PLUGINS)
    if(SCORE_BUILD_FOR_PACKAGE_MANAGER)
      install(TARGETS "${PluginName}"
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib)
    else()
      install(TARGETS "${PluginName}"
        LIBRARY DESTINATION .
        ARCHIVE DESTINATION static_lib
        RUNTIME DESTINATION bin)
    endif()
  endif()

  score_cotire_post("${PluginName}")
endfunction()


### Call with a plug-in target ###
function(setup_score_plugin PluginName)
  setup_score_common_lib_features("${PluginName}")

  set(SCORE_PLUGINS_LIST ${SCORE_PLUGINS_LIST} "${PluginName}" CACHE INTERNAL "List of plugins")

  set_target_properties(${PluginName} PROPERTIES
      LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/plugins/"
      RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/plugins/")
  if(NOT SCORE_STATIC_PLUGINS)
    if(SCORE_BUILD_FOR_PACKAGE_MANAGER)
      install(TARGETS "${PluginName}"
        LIBRARY DESTINATION lib/score
        ARCHIVE DESTINATION lib/score)
    else()
      install(TARGETS "${PluginName}"
        LIBRARY DESTINATION plugins
        ARCHIVE DESTINATION static_plugins
        RUNTIME DESTINATION bin/plugins)
    endif()
  endif()

  score_cotire_post("${PluginName}")
endfunction()
