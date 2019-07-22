#include <catch2/catch.hpp>
#include <gplc.h>


using namespace gplc;


TEST_CASE("Type System's tests")
{
	SECTION("TestGetDefaultValue_ReturnsCorrectTypeValue")
	{
		REQUIRE(dynamic_cast<CIntValue*>((new CType(CT_INT8, BTS_INT8, 0))->GetDefaultValue())->GetValue() == 0);
		REQUIRE(dynamic_cast<CIntValue*>((new CType(CT_INT16, BTS_INT16, 0x0))->GetDefaultValue())->GetValue() == 0);
		REQUIRE(dynamic_cast<CIntValue*>((new CType(CT_INT32, BTS_INT32, 0x0))->GetDefaultValue())->GetValue() == 0);
		REQUIRE(dynamic_cast<CIntValue*>((new CType(CT_INT64, BTS_INT64, 0x0))->GetDefaultValue())->GetValue() == 0);

		REQUIRE(dynamic_cast<CUIntValue*>((new CType(CT_UINT8, BTS_UINT8, 0x0))->GetDefaultValue())->GetValue() == 0);
		REQUIRE(dynamic_cast<CUIntValue*>((new CType(CT_UINT16, BTS_UINT16, 0x0))->GetDefaultValue())->GetValue() == 0);
		REQUIRE(dynamic_cast<CUIntValue*>((new CType(CT_UINT32, BTS_UINT32, 0x0))->GetDefaultValue())->GetValue() == 0);
		REQUIRE(dynamic_cast<CUIntValue*>((new CType(CT_UINT64, BTS_UINT64, 0x0))->GetDefaultValue())->GetValue() == 0);

		REQUIRE(dynamic_cast<CFloatValue*>((new CType(CT_FLOAT, BTS_FLOAT, 0x0))->GetDefaultValue())->GetValue() == Approx(0.0f));
		REQUIRE(dynamic_cast<CDoubleValue*>((new CType(CT_DOUBLE, BTS_DOUBLE, 0x0))->GetDefaultValue())->GetValue() == Approx(0.0));

		REQUIRE(dynamic_cast<CCharValue*>((new CType(CT_CHAR, BTS_CHAR, 0x0))->GetDefaultValue())->GetValue() == "\0");

		REQUIRE(dynamic_cast<CBoolValue*>((new CType(CT_BOOL, BTS_BOOL, 0x0))->GetDefaultValue())->GetValue() == true);

		REQUIRE(dynamic_cast<CPointerValue*>((new CFunctionType({}, nullptr, 0x0))->GetDefaultValue())->GetValue() == 0x0);
	}
}