#!/usr/bin/env python3

import conans
import glob

class ArduinoLibConan(conans.ConanFile):
    name = "samduino"
    url = "https://github.com/cheezypoofs/samduino"

    exports_sources = []

    generators = "cmake"
    settings = "os", "compiler", "arch", "build_type"
    description = "A library of arduino capabilities with tests"
    options = {}
    default_options = ()

    def imports(self):
        pass

    def requirements(self):
        self.requires("gtest/1.8.1@bincrafters/stable")

    def build(self):
        cmake = conans.CMake(self)
        cmake.configure(build_folder="build")
        cmake.build()
        cmake.test(output_on_failure=True)

    def package(self):
        self.copy(pattern="*.cpp", excludes="*Test*", src="lib")
        self.copy(pattern="*.h", src="lib")
