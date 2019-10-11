import os
import sys

from setuptools import setup, Extension

try:
    import pybind11
    pybind11_include_path = pybind11.get_include()
except ImportError:
    pybind11_include_path = ""


check_memory_leak = False

module_name = "indigo_astronomy"
ext_name = "__indigo"

py_ver = sys.version_info[:2]

include_dirs = [
    "../indigo_libs",
    pybind11_include_path,
]

libraries = [
    "indigo",
]

library_dirs = []

sources = []

source_path = "./src/"
for dirname, dirs, filenames in os.walk(source_path):
    for filename in sorted(filenames):
        if not filename.lower().endswith(".cpp"):
            continue
        sources.append(os.path.join(dirname, filename))
    break

extras = [
    "-std=c++14",
]

if sys.platform == "linux":
    extras.append("-DINDIGO_LINUX")
elif sys.platform == "darwin":
    extras.append("-DINDIGO_MAC")


extension = Extension(
    name="{0}.{1}".format(module_name, ext_name),
    include_dirs=include_dirs,
    libraries=libraries,
    library_dirs=library_dirs,
    sources=sources,
    extra_compile_args=extras,
)

BASE_PATH = os.path.dirname(os.path.realpath(__file__))

install_requires = []

requirements_path = os.path.join(BASE_PATH, "requirements.txt")

if os.path.exists(requirements_path):
    with open(requirements_path) as fp:
        install_requires = fp.read().splitlines()

setup(
    name=module_name,
    packages=[module_name],
    classifiers=[
        "Development Status :: 4 - Beta",
        "Intended Audience :: Developers",
        "Programming Language :: Python :: 3.6",
    ],
    ext_modules=[extension],
    install_requires=install_requires,
)
