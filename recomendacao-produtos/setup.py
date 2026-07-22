# ------------------------------------------------------------------
# Atividade 5 - Script de compilacao do modulo C++ para Python
#
# Uso (de preferencia no "x64 Native Tools Command Prompt for VS"):
#     python setup.py build_ext --inplace
#
# Isso gera um arquivo recomendador.<...>.pyd que pode ser importado
# no Python com:  import recomendador
# ------------------------------------------------------------------
from setuptools import setup
from pybind11.setup_helpers import Pybind11Extension, build_ext

ext_modules = [
    Pybind11Extension(
        "recomendador",
        [
            "src/bindings.cpp",
            "src/similaridade.cpp",
            "src/recomendacao.cpp",
        ],
        cxx_std=17,
    ),
]

setup(
    name="recomendador",
    version="1.0",
    description="Modulo de recomendacao em C++ exposto ao Python (pybind11)",
    ext_modules=ext_modules,
    cmdclass={"build_ext": build_ext},
)
