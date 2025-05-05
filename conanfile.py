from conan import ConanFile
from conan.tools.cmake import cmake_layout

class MyProjectConan(ConanFile):
    name = "TgControlPanel"
    version = "1.0.0"
    settings = "os", "arch", "compiler", "build_type"
    generators = "CMakeToolchain", "CMakeDeps"

    def layout(self):
        cmake_layout(self, build_folder="build")  
    default_options = {
        "sdl/*:shared": False,
        "boost/*:shared": False,
        "tgbot/*:shared": False,
        "sdl_mixer/*:shared": False,  
    }
    requires = [
        "sdl/2.28.5",
        "sdl_mixer/2.8.0",  
        "boost/1.83.0",
        "tgbot/1.8",
    ]

