import unittest

from tools.build_system.code_util import REPO_ROOT
from tools.build_system.module_organization import (
    BothNested,
    ModuleOrganization,
    RelativeNestedHeader,
    RelativeNestedSource,
    SameDirectory,
)


class TestModuleOrganization(unittest.TestCase):
    def test_module_organization_determine(self):
        InferredType = ModuleOrganization.determine("/a/b/c.cpp", "/a/b/c.h")
        self.assertEqual(InferredType, SameDirectory)

        InferredType = ModuleOrganization.determine("/a/b/src/c.cpp", "/a/b/c.h")
        self.assertEqual(InferredType, RelativeNestedSource)

        InferredType = ModuleOrganization.determine("/a/b/c.cpp", "/a/b/include/b/c.h")
        self.assertEqual(InferredType, RelativeNestedHeader)

        InferredType = ModuleOrganization.determine(
            "/a/b/src/c.cpp", "/a/b/include/b/c.h"
        )
        self.assertEqual(InferredType, BothNested)

    def test_same_directory(self):
        inferred_includepath = SameDirectory.includepath("/a/b/c/d.h")
        self.assertEqual(inferred_includepath, "-I/a/b/c")

    def test_both_nested(self):
        inferred_includepath = BothNested.includepath("/a/b/c/d.h")
        self.assertEqual(inferred_includepath, "-I/a/b")

    def test_relative_nested_header(self):
        inferred_includepath = RelativeNestedHeader.includepath("/a/b/c/include/c/d.h")
        self.assertEqual(inferred_includepath, "-I/a/b/c/include")

    def test_relative_nested_source(self):
        inferred_includepath = RelativeNestedSource.includepath("/a/b/c/d.h")
        self.assertEqual(inferred_includepath, f"-I{REPO_ROOT}")


if __name__ == "__main__":
    unittest.main()
