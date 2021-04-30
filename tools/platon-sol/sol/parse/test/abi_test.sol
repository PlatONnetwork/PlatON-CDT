// SPDX-License-Identifier: MIT

pragma solidity ^0.8.2;
contract AbiTest {

    struct SA { // Struct
        uint[] a;
        string b;
        int[] c;
        bytes24 d;
        bytes32 e;
        bytes12 f;
        uint128 g;
        int8[] h;
        bool i;
    }

    struct SB { // Struct
        int16 a;
        SA b;
    }

    struct SC { // Struct
        int32 a;
        int64 b;
        SB c;
    }

    struct SD { // Struct
        int32[] a;
        string[] b;
        SC[] c;
        SC[2] d;
    }


    function setA(SA memory v) public returns (SA memory) {}
    function setB(SB memory v) public returns (SB memory) {}
    function setC(SC memory v) public returns (SC memory) {}
    function setD(SD memory v) public returns (SD memory) {}
    function setAD(SA memory v,SD memory v2) public {}
    function setInt(uint8 a, uint32 b, int128 c, int256 d) public returns(uint8, uint32, int128, int256){}
    function setIntArray(uint32[] memory a, int256[] memory b) public returns(uint32[] memory, int256[] memory){}
    function setArray(bytes12 a, bytes18 b) public returns(bytes24, bytes32){}
    function setString(string memory a, string memory b) public returns(string memory, string memory){}
    function setBytes(bytes memory a, bytes memory b) public returns(bytes memory, bytes memory){}
    function setStruct(bytes memory a, uint32 b, SB memory c) public returns(SB memory, bytes memory, uint32){}
}