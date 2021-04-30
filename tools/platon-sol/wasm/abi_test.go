package wasm

import (
	"testing"

	"gotest.tools/assert"
)

func TestWasmMarshal(t *testing.T) {
	abiStr := `[{"anonymous":false,"input":[{"name":"topic1","type":"uint128"},{"name":"topic2","type":"FixedHash<20>"},{"name":"arg1","type":"uint64"},{"name":"arg2","type":"uint128"},{"name":"arg3","type":"uint16"},{"name":"arg4","type":"uint16"},{"name":"arg5","type":"uint128"}],"name":"LogCampaignAdded","topic":2,"type":"Event"},{"constant":false,"input":[{"name":"campaign_id","type":"uint128"},{"name":"s","type":"uint128"}],"name":"Reveal","output":"void","type":"Action"},{"constant":false,"input":[{"name":"campaign_id","type":"uint128"}],"name":"GetMyBounty","output":"void","type":"Action"},{"constant":false,"input":[{"name":"campaign_id","type":"uint128"}],"name":"RefundBounty","output":"void","type":"Action"},{"constant":true,"input":[{"name":"campaign_id","type":"uint128"}],"name":"GetRandom","output":"uint128","type":"Action"},{"anonymous":false,"input":[{"name":"topic1","type":"uint128"},{"name":"topic2","type":"FixedHash<20>"},{"name":"arg1","type":"uint128"}],"name":"LogFollow","topic":2,"type":"Event"},{"anonymous":false,"input":[{"name":"topic1","type":"uint128"},{"name":"topic2","type":"FixedHash<20>"},{"name":"arg1","type":"uint8[32]"}],"name":"LogCommit","topic":2,"type":"Event"},{"anonymous":false,"input":[{"name":"topic1","type":"uint128"},{"name":"topic2","type":"FixedHash<20>"},{"name":"arg1","type":"uint128"}],"name":"LogReveal","topic":2,"type":"Event"},{"constant":false,"input":[],"name":"init","output":"void","type":"Action"},{"constant":false,"input":[{"name":"burn_num","type":"uint64"},{"name":"deposit","type":"uint128"},{"name":"commit_balkline","type":"uint16"},{"name":"commit_deadline","type":"uint16"}],"name":"NewCampaign","output":"uint128","type":"Action"},{"constant":false,"input":[{"name":"campaign_id","type":"uint128"}],"name":"Follow","output":"void","type":"Action"},{"constant":false,"input":[{"name":"campaign_id","type":"uint128"},{"name":"hs","type":"uint8[32]"}],"name":"Commit","output":"void","type":"Action"}]`
	abi, err := NewABI(abiStr)

	assert.NilError(t, err)
	assert.Equal(t, len(abi.Methods), 8)
	assert.Equal(t, len(abi.Structs), 0)

	abiStr = `[{"baseclass":[],"fields":[{"name":"arr","type":"int32[5]"}],"name":"my_array","type":"struct"},{"constant":true,"input":[],"name":"get_my_array","output":"my_array","type":"Action"},{"anonymous":false,"input":[{"name":"topic","type":"string"},{"name":"arg1","type":"string"},{"name":"arg2","type":"uint32"}],"name":"hello_event","topic":1,"type":"Event"},{"baseclass":[],"fields":[{"name":"head","type":"string"}],"name":"message","type":"struct"},{"baseclass":["message"],"fields":[{"name":"body","type":"string"},{"name":"end","type":"string"}],"name":"my_message","type":"struct"},{"constant":false,"input":[{"name":"one_message","type":"my_message"}],"name":"init","output":"void","type":"Action"},{"constant":false,"input":[{"name":"one_message","type":"my_message"}],"name":"add_message","output":"my_message[]","type":"Action"},{"constant":true,"input":[{"name":"name","type":"string"}],"name":"get_message","output":"my_message[]","type":"Action"},{"baseclass":[],"fields":[{"name":"vec","type":"uint8[][]"}],"name":"my_vector","type":"struct"},{"constant":true,"input":[],"name":"get_my_vector","output":"my_vector","type":"Action"},{"baseclass":[],"fields":[{"name":"mp","type":"map<uint32,string>"}],"name":"my_map","type":"struct"},{"constant":true,"input":[],"name":"get_my_map","output":"my_map","type":"Action"},{"constant":false,"input":[],"name":"hello_abort","output":"void","type":"Action"},{"constant":false,"input":[],"name":"hello_panic","output":"void","type":"Action"}]`
	abi, err = NewABI(abiStr)
	assert.NilError(t, err)
	assert.Equal(t, len(abi.Methods), 8)
	assert.Equal(t, len(abi.Structs), 5)
}
