
#include "llvm/ADT/StringRef.h"
#include "llvm/AsmParser/Parser.h"
#include "llvm/AsmParser/SlotMapping.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/Support/SourceMgr.h"
#include "llvm/IR/Metadata.h"
#include "llvm/IR/DebugInfoMetadata.h"
#include "llvm/Support/JSON.h"
#include "../MakeAbi/MakeAbi.h"
#include "unit_test.hpp"

using namespace llvm;
using namespace llvm::json;

StringRef getName(DINode* Node);
bool isString(DICompositeType* DerT);
bool isVector(DICompositeType* CT);
bool isFixedHash(DICompositeType* CT);

TEST(ABITest, StringTest) {
  LLVMContext Ctx;
  StringRef Source = R"(
    !0 = !DICompositeType(tag: DW_TAG_class_type, name: "basic_string<char, std::__1::char_traits<char>, std::__1::allocator<char> >", identifier: "_ZTSNSt3__112basic_stringIcNS_11char_traitsIcEENS_9allocatorIcEEEE")
    )";

  SMDiagnostic Error;
  SlotMapping Mapping;
  auto Mod = parseAssemblyString(Source, Error, Ctx, &Mapping);

  EXPECT_TRUE(Error.getMessage().empty());

  DICompositeType* DT= cast<DICompositeType>(Mapping.MetadataNodes[0].get());
  EXPECT_TRUE(isString(DT));
}

TEST(ABITest, VectorTest) {
  LLVMContext Ctx;
  StringRef Source = R"(
    !0 = distinct !DICompositeType(tag: DW_TAG_class_type, name: "vector<int, std::__1::allocator<int> >", templateParams: !4, identifier: "_ZTSNSt3__16vectorIiNS_9allocatorIiEEEE")
    
    !4 = !{!5, !6}
    !5 = !DITemplateTypeParameter(name: "_Tp", type: !7)
    !6 = !DITemplateTypeParameter(name: "_Allocator", type: !7)
    !7 = !DIBasicType(name: "int", size: 32, encoding: DW_ATE_signed)
    )";

  SMDiagnostic Error;
  SlotMapping Mapping;
  auto Mod = parseAssemblyString(Source, Error, Ctx, &Mapping);

  EXPECT_TRUE(Error.getMessage().empty());

  DICompositeType* DT = cast<DICompositeType>(Mapping.MetadataNodes[0].get());
  EXPECT_TRUE(isVector(DT));


  MakeAbi MABI;
  EXPECT_EQ(MABI.handleVector(nullptr, DT), "int32[]");
}

TEST(ABITest, FixedHashTest) {
  LLVMContext Ctx;
  StringRef Source = R"(
    !0 = distinct !DICompositeType(tag: DW_TAG_class_type, name: "FixedHash<20>", templateParams: !1, identifier: "_ZTSN6platon9FixedHashILj20EEE")
    !1 = !{!2}
    !2 = !DITemplateValueParameter(name: "N", type: !3, value: i32 20)
    !3 = !DIBasicType(name: "unsigned int", size: 32, encoding: DW_ATE_unsigned)
   )";

  SMDiagnostic Error;
  SlotMapping Mapping;
  auto Mod = parseAssemblyString(Source, Error, Ctx, &Mapping);

  EXPECT_TRUE(Error.getMessage().empty());

  DICompositeType* DT = cast<DICompositeType>(Mapping.MetadataNodes[0].get());
  EXPECT_TRUE(isFixedHash(DT));


  MakeAbi MABI;
  EXPECT_EQ(MABI.handleFixedHash(nullptr, DT), "FixedHash<20>");
}

TEST(ABITest, getNameTest) {
  LLVMContext Ctx;
  StringRef Source = R"(
    !0 = !DILocalVariable(name: "i", arg: 2, line: 37, scope: !3)
    !1 = !DIDerivedType(tag: DW_TAG_member, name: "n", baseType: !2, size: 32)
    !2 = !DIBasicType(name: "int", size: 32, encoding: DW_ATE_signed)
    !3 = !{}
   )";

  SMDiagnostic Error;
  SlotMapping Mapping;
  auto Mod = parseAssemblyString(Source, Error, Ctx, &Mapping);

  llvm::errs() << Error.getMessage() << "\n";
  EXPECT_TRUE(Error.getMessage().empty());

  DILocalVariable* LV = cast<DILocalVariable>(Mapping.MetadataNodes[0].get());
  DIDerivedType* Member = cast<DIDerivedType>(Mapping.MetadataNodes[1].get());
  EXPECT_EQ(getName(LV), "i");
  EXPECT_EQ(getName(Member), "n");
}

TEST(ABITest, BasicTypeTest) {
  LLVMContext Ctx;
  StringRef Source =
    "!0 = !DIBasicType(name: \"bool\", size: 8, encoding: DW_ATE_boolean)"
    "!1 = !DIBasicType(name: \"char\", size: 8, encoding: DW_ATE_signed_char)"
    "!2 = !DIBasicType(name: \"short\", size: 16, encoding: DW_ATE_signed)"
    "!3 = !DIBasicType(name: \"int\", size: 32, encoding: DW_ATE_signed)"
    "!4 = !DIBasicType(name: \"long long int\", size: 64, encoding: DW_ATE_signed)"

    "!5 = !DIBasicType(name: \"unsigned char\", size: 8, encoding: DW_ATE_unsigned_char)"
    "!6 = !DIBasicType(name: \"unsigned short\", size: 16, encoding: DW_ATE_unsigned)"
    "!7 = !DIBasicType(name: \"unsigned int\", size: 32, encoding: DW_ATE_unsigned)"
    "!8 = !DIBasicType(name: \"long long unsigned int\", size: 64, encoding: DW_ATE_unsigned)";

  SMDiagnostic Error;
  SlotMapping Mapping;
  auto Mod = parseAssemblyString(Source, Error, Ctx, &Mapping);

  EXPECT_EQ(Mapping.MetadataNodes.size(), 9);

  MakeAbi MABI;
  StringRef BasicTypeName[] = {"bool", "int8", "int16", "int32", "int64", "uint8", "uint16", "uint32", "uint64"};

  for(unsigned i=0; i<9; i++){
    DIBasicType* BT = cast<DIBasicType>(Mapping.MetadataNodes[i].get());
    EXPECT_EQ(MABI.handleBasicType(nullptr, BT), BasicTypeName[i]);
  }
}

TEST(ABITest, handleElemTest) {
  LLVMContext Ctx;
  StringRef Source = R"(
    !0 = !DIDerivedType(tag: DW_TAG_member, name: "n", baseType: !1, size: 32)
    !1 = !DIBasicType(name: "int", size: 32, encoding: DW_ATE_signed)
    )";

  SMDiagnostic Error;
  SlotMapping Mapping;
  auto Mod = parseAssemblyString(Source, Error, Ctx, &Mapping);

  MakeAbi MABI;

  DIDerivedType* DT = cast<DIDerivedType>(Mapping.MetadataNodes[0].get());

  json::Value result = Object {{"name", "n"}, {"type", "int32"}};

  EXPECT_EQ(MABI.handleElem(DT, DT->getBaseType()), result);
}

TEST(ABITest, handleDerivedTypeTest) {
  LLVMContext Ctx;
  StringRef Source = R"(
    !0 = !DIBasicType(name: "int", size: 32, encoding: DW_ATE_signed)
    !1 = !DIDerivedType(tag: DW_TAG_reference_type, baseType: !0, size: 32)
    !2 = !DIDerivedType(tag: DW_TAG_typedef, name: "qint", line: 25, baseType: !1)
    !3 = !DIDerivedType(tag: DW_TAG_const_type, baseType: !0)
    )";

  SMDiagnostic Error;
  SlotMapping Mapping;
  auto Mod = parseAssemblyString(Source, Error, Ctx, &Mapping);

  MakeAbi MABI;

  DIDerivedType* ref = cast<DIDerivedType>(Mapping.MetadataNodes[1].get());
  DIDerivedType* tydef = cast<DIDerivedType>(Mapping.MetadataNodes[2].get());
  DIDerivedType* con = cast<DIDerivedType>(Mapping.MetadataNodes[3].get());

  EXPECT_EQ(MABI.handleDerivedType(nullptr, ref), "int32");
  EXPECT_EQ(MABI.handleDerivedType(nullptr, tydef), "int32");
  EXPECT_EQ(MABI.handleDerivedType(nullptr, con), "int32");
}

TEST(ABITest, handleStructTypeTest) {
  LLVMContext Ctx;
  StringRef Source = R"(
    !0 = distinct !DICompositeType(tag: DW_TAG_structure_type, name: "N", line: 12, size: 64, flags: DIFlagTypePassByValue | DIFlagTrivial, elements: !1, identifier: "_ZTS1N")
    !1 = !{!2, !3}
    !2 = !DIDerivedType(tag: DW_TAG_member, name: "a", scope: !0, baseType: !4, size: 32)
    !3 = !DIDerivedType(tag: DW_TAG_member, name: "b", scope: !0, baseType: !5, size: 8, offset: 32)
    !4 = !DIBasicType(name: "int", size: 32, encoding: DW_ATE_signed)
    !5 = !DIBasicType(name: "char", size: 8, encoding: DW_ATE_signed_char)
    )";

  SMDiagnostic Error;
  SlotMapping Mapping;
  auto Mod = parseAssemblyString(Source, Error, Ctx, &Mapping);

  MakeAbi MABI;

  DICompositeType* CT = cast<DICompositeType>(Mapping.MetadataNodes[0].get());

  StringRef name = MABI.handleStructType(nullptr, CT);
  EXPECT_EQ(name, "N");

  json::Value result = MABI.contents.getAsArray()->back();
  json::Value v = Object {
      {"baseclass", Array{}},
      {"fields", Array{
            Object {{"name", "a"}, {"type", "int32"}},
            Object {{"name", "b"}, {"type", "int8"}}}},
      {"name", "N"},
      {"type", "struct"}
    };

  EXPECT_EQ(result, v);
}

UNITTEST_MAIN() {
  RUN_TEST(ABITest, StringTest);
  RUN_TEST(ABITest, VectorTest);
  RUN_TEST(ABITest, FixedHashTest);
  RUN_TEST(ABITest, getNameTest);
  RUN_TEST(ABITest, BasicTypeTest);
  RUN_TEST(ABITest, handleElemTest);
  RUN_TEST(ABITest, handleDerivedTypeTest);
  //RUN_TEST(ABITest, handleStructTypeTest);
}

