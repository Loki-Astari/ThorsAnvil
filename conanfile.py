from conan import ConanFile, tools
from conan.tools.scm import Git
from conan.tools.gnu import Autotools
from conan.tools.gnu import AutotoolsDeps
from conan.tools.gnu import AutotoolsToolchain

class ThorsAnvilConan(ConanFile):
    name = "thors-anvil"
    version = "9.1.11"
    license = "MIT"
    author = "Martin York Loki.Astari@gmail.com"
    url = "https://github.com/Loki-Astari/ThorsAnvil"
    description = "C++20 monorepo: declarative serialization (JSON/YAML/BSON), async IO, coroutine HTTP server, MongoDB client, Slack client, and hot-loadable plugin framework"
    topics = ("C++", "Serialization", "Mongo", "JSON", "BSON", "YAML", "HTTP", "Async", "Coroutines")
    settings = "os", "compiler", "build_type", "arch"
    options = {"shared": [True, False], "fPIC": [True, False]}
    default_options = {"shared": True, "fPIC": True}

    def generate(self):
        tc = AutotoolsToolchain(self)
        tc.configure_args.append("--enable-test-with-conan")
        tc.configure_args.append("--enable-dark-mode")
        tc.configure_args.append("--disable-test-with-mongo-query")
        tc.configure_args.append("--disable-Mongo-Service")
        tc.configure_args.append("--disable-vera")
        print("ZLib:    " + self.dependencies["zlib"].package_folder)
        tc.configure_args.append("--with-zlib-root=" + self.dependencies["zlib"].package_folder)
        print("Yaml:    " + self.dependencies["libyaml"].package_folder)
        tc.configure_args.append("--with-yaml-root=" + self.dependencies["libyaml"].package_folder)
        print("Snappy:  " + self.dependencies["snappy"].package_folder)
        tc.configure_args.append("--with-snappy-root=" + self.dependencies["snappy"].package_folder)
        print("Open:    " + self.dependencies["openssl"].package_folder)
        tc.configure_args.append("--with-crypto-root=" + self.dependencies["openssl"].package_folder)
        print("Magic:   " + self.dependencies["magic_enum"].package_folder)
        tc.configure_args.append("--with-magicenum-header-only-root=" + self.dependencies["magic_enum"].package_folder + "/include/magic_enum")
        print("Event:   " + self.dependencies["libevent"].package_folder)
        tc.configure_args.append("--with-event-root=" + self.dependencies["libevent"].package_folder)
        tc.generate()

    def requirements(self):
        self.requires("bzip2/1.0.8")
        self.requires("zlib/1.3.1")
        self.requires("libyaml/0.2.5")
        self.requires("snappy/1.2.1")
        self.requires("openssl/3.4.1")
        self.requires("magic_enum/0.9.7")
        self.requires("boost/1.86.0")
        self.requires("libevent/2.1.12")

    def source(self):
        git = Git(self)
        git.clone(url="https://github.com/Loki-Astari/ThorsAnvil.git", target=".")

    def validate(self):
        tools.build.check_min_cppstd(self, 20)

    def build(self):
        autotools = Autotools(self)
        autotools.configure()
        autotools.make()

    def package(self):
        autotools = Autotools(self)
        autotools.install()

    def package_info(self):
        self.cpp_info.libs = [  "ThorSerialize",    "ThorsLogging",    "ThorsMongo",    "ThorsSocket",    "ThorsStorage",    "Nisse",       # Release Sym
                                "ThorSerializeD",   "ThorsLoggingD",   "ThorsMongoD",   "ThorsSocketD",   "ThorsStorageD",   "NisseD",      # Debug   Sym
                                "ThorSerialize20",  "ThorsLogging20",  "ThorsMongo20",  "ThorsSocket20",  "ThorsStorage20",  "Nisse20",     # Release Versioned
                                "ThorSerialize20D", "ThorsLogging20D", "ThorsMongo20D", "ThorsSocket20D", "ThorsStorage20D", "Nisse20D"     # Debug  Versioned
                             ]
        self.cpp_info.libdirs = ["lib"]
        self.cpp_info.includedirs = [ "include",
                                      "include/ThorSerialize",
                                      "include/ThorsCrypto",
                                      "include/ThorsIOUtil",
                                      "include/ThorsLogging",
                                      "include/ThorsMongo",
                                      "include/ThorsSocket",
                                      "include/ThorsStorage",
                                      "include/NisseHTTP",
                                      "include/NisseServer",
                                      "include/ThorsMug",
                                      "include/ThorsSlack",
                                      "include/fast_float"
                                    ]
