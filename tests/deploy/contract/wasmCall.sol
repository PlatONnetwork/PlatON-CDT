pragma solidity ^0.5.17;
pragma experimental ABIEncoderV2;
library RLPEncode {
    // Hash the name of the method to be called
    function hash(string memory str) internal pure returns (uint64) {
        uint64 kOffset64 = 14695981039346656037;
        uint64 kPrime64 = 1099511628211;

        bytes memory method = bytes(str);
        uint64 value = kOffset64;
        for (uint i = 0; i < method.length; i++) {
            value *= kPrime64;
            value ^= uint8(method[i]);
        }
        return value;
    }

    /*
     * Internal functions
     */

    /**
     * @dev RLP encodes a byte string.
     * @param self The byte string to encode.
     * @return The RLP encoded string in bytes.
     */
    function encodeBytes(bytes memory self)
        internal
        pure
        returns (bytes memory)
    {
        bytes memory encoded;
        if (self.length == 1 && uint8(self[0]) <= 128) {
            encoded = self;
        } else {
            encoded = concat(encodeLength(self.length, 128), self);
        }
        return encoded;
    }

    /**
     * @dev RLP encodes a list of RLP encoded byte byte strings.
     * @param self The list of RLP encoded byte strings.
     * @return The RLP encoded list of items in bytes.
     */
    function encodeList(bytes[] memory self)
        internal
        pure
        returns (bytes memory)
    {
        bytes memory list = flatten(self);
        return concat(encodeLength(list.length, 192), list);
    }

    /**
     * @dev RLP encodes a string.
     * @param self The string to encode.
     * @return The RLP encoded string in bytes.
     */
    function encodeString(string memory self)
        internal
        pure
        returns (bytes memory)
    {
        return encodeBytes(bytes(self));
    }

    /**
     * @dev RLP encodes an address.
     * @param self The address to encode.
     * @return The RLP encoded address in bytes.
     */
    function encodeAddress(address self) internal pure returns (bytes memory) {
        bytes memory inputBytes;
        assembly {
            let m := mload(0x40)
            mstore(
                add(m, 20),
                xor(0x140000000000000000000000000000000000000000, self)
            )
            mstore(0x40, add(m, 52))
            inputBytes := m
        }
        return encodeBytes(inputBytes);
    }

    /**
     * @dev RLP encodes a uint.
     * @param self The uint to encode.
     * @return The RLP encoded uint in bytes.
     */
    function encodeUint(uint256 self) internal pure returns (bytes memory) {
        return encodeBytes(toBinary(self));
    }

    /**
     * @dev RLP encodes an int.
     * @param self The int to encode.
     * @return The RLP encoded int in bytes.
     */
    function encodeInt(int256 self) internal pure returns (bytes memory) {
        return encodeUint(uint256((self << 1) ^ (self >> 255)));
    }

    /**
     * @dev RLP encodes a bool.
     * @param self The bool to encode.
     * @return The RLP encoded bool in bytes.
     */
    function encodeBool(bool self) internal pure returns (bytes memory) {
        bytes memory encoded = new bytes(1);
        encoded[0] = (self ? bytes1(0x01) : bytes1(0x80));
        return encoded;
    }

    /*
     * Private functions
     */

    /**
     * @dev Encode the first byte, followed by the len in binary form if length is more than 55.
     * @param len The length of the string or the payload.
     * @param offset 128 if item is string, 192 if item is list.
     * @return RLP encoded bytes.
     */
    function encodeLength(uint256 len, uint256 offset)
        private
        pure
        returns (bytes memory)
    {
        bytes memory encoded;
        if (len < 56) {
            encoded = new bytes(1);
            encoded[0] = bytes32(len + offset)[31];
        } else {
            uint256 lenLen;
            uint256 i = 1;
            while (len / i != 0) {
                lenLen++;
                i *= 256;
            }

            encoded = new bytes(lenLen + 1);
            encoded[0] = bytes32(lenLen + offset + 55)[31];
            for (i = 1; i <= lenLen; i++) {
                encoded[i] = bytes32((len / (256**(lenLen - i))) % 256)[31];
            }
        }
        return encoded;
    }

    /**
     * @dev Encode integer in big endian binary form with no leading zeroes.
     * @notice TODO: This should be optimized with assembly to save gas costs.
     * @param _x The integer to encode.
     * @return RLP encoded bytes.
     */
    function toBinary(uint256 _x) private pure returns (bytes memory) {
        bytes memory b = new bytes(32);
        assembly {
            mstore(add(b, 32), _x)
        }
        uint256 i;
        for (i = 0; i < 32; i++) {
            if (b[i] != 0) {
                break;
            }
        }
        bytes memory res = new bytes(32 - i);
        for (uint256 j = 0; j < res.length; j++) {
            res[j] = b[i++];
        }
        return res;
    }

    /**
     * @dev Copies a piece of memory to another location.
     * @notice From: https://github.com/Arachnid/solidity-stringutils/blob/master/src/strings.sol.
     * @param _dest Destination location.
     * @param _src Source location.
     * @param _len Length of memory to copy.
     */
    function memcpy(
        uint256 _dest,
        uint256 _src,
        uint256 _len
    ) private pure {
        uint256 dest = _dest;
        uint256 src = _src;
        uint256 len = _len;

        for (; len >= 32; len -= 32) {
            assembly {
                mstore(dest, mload(src))
            }
            dest += 32;
            src += 32;
        }

        uint256 mask = 256**(32 - len) - 1;
        assembly {
            let srcpart := and(mload(src), not(mask))
            let destpart := and(mload(dest), mask)
            mstore(dest, or(destpart, srcpart))
        }
    }

    /**
     * @dev Flattens a list of byte strings into one byte string.
     * @notice From: https://github.com/sammayo/solidity-rlp-encoder/blob/master/RLPEncode.sol.
     * @param _list List of byte strings to flatten.
     * @return The flattened byte string.
     */
    function flatten(bytes[] memory _list) private pure returns (bytes memory) {
        if (_list.length == 0) {
            return new bytes(0);
        }

        uint256 len;
        uint256 i;
        for (i = 0; i < _list.length; i++) {
            len += _list[i].length;
        }

        bytes memory flattened = new bytes(len);
        uint256 flattenedPtr;
        assembly {
            flattenedPtr := add(flattened, 0x20)
        }

        for (i = 0; i < _list.length; i++) {
            bytes memory item = _list[i];

            uint256 listPtr;
            assembly {
                listPtr := add(item, 0x20)
            }

            memcpy(flattenedPtr, listPtr, item.length);
            flattenedPtr += _list[i].length;
        }

        return flattened;
    }

    /**
     * @dev Concatenates two bytes.
     * @notice From: https://github.com/GNSPS/solidity-bytes-utils/blob/master/contracts/BytesLib.sol.
     * @param _preBytes First byte string.
     * @param _postBytes Second byte string.
     * @return Both byte string combined.
     */
    function concat(bytes memory _preBytes, bytes memory _postBytes)
        private
        pure
        returns (bytes memory)
    {
        bytes memory tempBytes;

        assembly {
            tempBytes := mload(0x40)

            let length := mload(_preBytes)
            mstore(tempBytes, length)

            let mc := add(tempBytes, 0x20)
            let end := add(mc, length)

            for {
                let cc := add(_preBytes, 0x20)
            } lt(mc, end) {
                mc := add(mc, 0x20)
                cc := add(cc, 0x20)
            } {
                mstore(mc, mload(cc))
            }

            length := mload(_postBytes)
            mstore(tempBytes, add(length, mload(tempBytes)))

            mc := end
            end := add(mc, length)

            for {
                let cc := add(_postBytes, 0x20)
            } lt(mc, end) {
                mc := add(mc, 0x20)
                cc := add(cc, 0x20)
            } {
                mstore(mc, mload(cc))
            }

            mstore(
                0x40,
                and(
                    add(add(end, iszero(add(length, mload(_preBytes)))), 31),
                    not(31)
                )
            )
        }

        return tempBytes;
    }
}

library RLPReader {
    uint8 constant STRING_SHORT_START = 0x80;
    uint8 constant STRING_LONG_START  = 0xb8;
    uint8 constant LIST_SHORT_START   = 0xc0;
    uint8 constant LIST_LONG_START    = 0xf8;
    uint8 constant WORD_SIZE = 32;

    struct RLPItem {
        uint len;
        uint memPtr;
    }

    struct Iterator {
        RLPItem item;   // Item that's being iterated over.
        uint nextPtr;   // Position of the next item in the list.
    }

    /*
    * @dev Returns the next element in the iteration. Reverts if it has not next element.
    * @param self The iterator.
    * @return The next element in the iteration.
    */
    function next(Iterator memory self) internal pure returns (RLPItem memory) {
        require(hasNext(self));

        uint ptr = self.nextPtr;
        uint itemLength = _itemLength(ptr);
        self.nextPtr = ptr + itemLength;

        return RLPItem(itemLength, ptr);
    }

    /*
    * @dev Returns true if the iteration has more elements.
    * @param self The iterator.
    * @return true if the iteration has more elements.
    */
    function hasNext(Iterator memory self) internal pure returns (bool) {
        RLPItem memory item = self.item;
        return self.nextPtr < item.memPtr + item.len;
    }

    /*
    * @param item RLP encoded bytes
    */
    function toRlpItem(bytes memory item) internal pure returns (RLPItem memory) {
        uint memPtr;
        assembly {
            memPtr := add(item, 0x20)
        }

        return RLPItem(item.length, memPtr);
    }

    /*
    * @dev Create an iterator. Reverts if item is not a list.
    * @param self The RLP item.
    * @return An 'Iterator' over the item.
    */
    function iterator(RLPItem memory self) internal pure returns (Iterator memory) {
        require(isList(self));

        uint ptr = self.memPtr + _payloadOffset(self.memPtr);
        return Iterator(self, ptr);
    }

    /*
    * @param the RLP item.
    */
    function rlpLen(RLPItem memory item) internal pure returns (uint) {
        return item.len;
    }

    /*
     * @param the RLP item.
     * @return (memPtr, len) pair: location of the item's payload in memory.
     */
    function payloadLocation(RLPItem memory item) internal pure returns (uint, uint) {
        uint offset = _payloadOffset(item.memPtr);
        uint memPtr = item.memPtr + offset;
        uint len = item.len - offset; // data length
        return (memPtr, len);
    }

    /*
    * @param the RLP item.
    */
    function payloadLen(RLPItem memory item) internal pure returns (uint) {
        (, uint len) = payloadLocation(item);
        return len;
    }

    /*
    * @param the RLP item containing the encoded list.
    */
    function toList(RLPItem memory item) internal pure returns (RLPItem[] memory) {
        require(isList(item));

        uint items = numItems(item);
        RLPItem[] memory result = new RLPItem[](items);

        uint memPtr = item.memPtr + _payloadOffset(item.memPtr);
        uint dataLen;
        for (uint i = 0; i < items; i++) {
            dataLen = _itemLength(memPtr);
            result[i] = RLPItem(dataLen, memPtr); 
            memPtr = memPtr + dataLen;
        }

        return result;
    }

    // @return indicator whether encoded payload is a list. negate this function call for isData.
    function isList(RLPItem memory item) internal pure returns (bool) {
        if (item.len == 0) return false;

        uint8 byte0;
        uint memPtr = item.memPtr;
        assembly {
            byte0 := byte(0, mload(memPtr))
        }

        if (byte0 < LIST_SHORT_START)
            return false;
        return true;
    }

    /*
     * @dev A cheaper version of keccak256(toRlpBytes(item)) that avoids copying memory.
     * @return keccak256 hash of RLP encoded bytes.
     */
    function rlpBytesKeccak256(RLPItem memory item) internal pure returns (bytes32) {
        uint256 ptr = item.memPtr;
        uint256 len = item.len;
        bytes32 result;
        assembly {
            result := keccak256(ptr, len)
        }
        return result;
    }

    /*
     * @dev A cheaper version of keccak256(toBytes(item)) that avoids copying memory.
     * @return keccak256 hash of the item payload.
     */
    function payloadKeccak256(RLPItem memory item) internal pure returns (bytes32) {
        (uint memPtr, uint len) = payloadLocation(item);
        bytes32 result;
        assembly {
            result := keccak256(memPtr, len)
        }
        return result;
    }

    /** RLPItem conversions into data types **/

    // @returns raw rlp encoding in bytes
    function toRlpBytes(RLPItem memory item) internal pure returns (bytes memory) {
        bytes memory result = new bytes(item.len);
        if (result.length == 0) return result;
        
        uint ptr;
        assembly {
            ptr := add(0x20, result)
        }

        copy(item.memPtr, ptr, item.len);
        return result;
    }

    // any non-zero byte except "0x80" is considered true
    function toBoolean(RLPItem memory item) internal pure returns (bool) {
        require(item.len == 1);
        uint result;
        uint memPtr = item.memPtr;
        assembly {
            result := byte(0, mload(memPtr))
        }

        // SEE Github Issue #5.
        // Summary: Most commonly used RLP libraries (i.e Geth) will encode
        // "0" as "0x80" instead of as "0". We handle this edge case explicitly
        // here.
        if (result == 0 || result == STRING_SHORT_START) {
            return false;
        } else {
            return true;
        }
    }

    function toAddress(RLPItem memory item) internal pure returns (address) {
        // 1 byte for the length prefix
        require(item.len == 21);

        return address(toUint(item));
    }

    function toUint(RLPItem memory item) internal pure returns (uint) {
        require(item.len > 0 && item.len <= 33);

        (uint memPtr, uint len) = payloadLocation(item);

        uint result;
        assembly {
            result := mload(memPtr)

            // shfit to the correct location if neccesary
            if lt(len, 32) {
                result := div(result, exp(256, sub(32, len)))
            }
        }

        return result;
    }

    function toInt(RLPItem memory item) internal pure returns (int) {
        require(item.len > 0 && item.len <= 33);

        (uint memPtr, uint len) = payloadLocation(item);

        uint result;
        assembly {
            result := mload(memPtr)

            // shfit to the correct location if neccesary
            if lt(len, 32) {
                result := div(result, exp(256, sub(32, len)))
            }
        }

        int real = int((result >> 1) ^ -(result & 1));

        return real;
    }

    // enforces 32 byte length
    function toUintStrict(RLPItem memory item) internal pure returns (uint) {
        // one byte prefix
        require(item.len == 33);

        uint result;
        uint memPtr = item.memPtr + 1;
        assembly {
            result := mload(memPtr)
        }

        return result;
    }

    function toBytes(RLPItem memory item) internal pure returns (bytes memory) {
        require(item.len > 0);

        (uint memPtr, uint len) = payloadLocation(item);
        bytes memory result = new bytes(len);

        uint destPtr;
        assembly {
            destPtr := add(0x20, result)
        }

        copy(memPtr, destPtr, len);
        return result;
    }

    function toString(RLPItem memory item) internal pure returns (string memory) {
        require(item.len > 0);

        (uint memPtr, uint len) = payloadLocation(item);
        bytes memory result = new bytes(len);

        uint destPtr;
        assembly {
            destPtr := add(0x20, result)
        }

        copy(memPtr, destPtr, len);

        return string(result);
    }

    /*
    * Private Helpers
    */

    // @return number of payload items inside an encoded list.
    function numItems(RLPItem memory item) private pure returns (uint) {
        if (item.len == 0) return 0;

        uint count = 0;
        uint currPtr = item.memPtr + _payloadOffset(item.memPtr);
        uint endPtr = item.memPtr + item.len;
        while (currPtr < endPtr) {
           currPtr = currPtr + _itemLength(currPtr); // skip over an item
           count++;
        }

        return count;
    }

    // @return entire rlp item byte length
    function _itemLength(uint memPtr) private pure returns (uint) {
        uint itemLen;
        uint byte0;
        assembly {
            byte0 := byte(0, mload(memPtr))
        }

        if (byte0 < STRING_SHORT_START)
            itemLen = 1;
        
        else if (byte0 < STRING_LONG_START)
            itemLen = byte0 - STRING_SHORT_START + 1;

        else if (byte0 < LIST_SHORT_START) {
            assembly {
                let byteLen := sub(byte0, 0xb7) // # of bytes the actual length is
                memPtr := add(memPtr, 1) // skip over the first byte
                
                /* 32 byte word size */
                let dataLen := div(mload(memPtr), exp(256, sub(32, byteLen))) // right shifting to get the len
                itemLen := add(dataLen, add(byteLen, 1))
            }
        }

        else if (byte0 < LIST_LONG_START) {
            itemLen = byte0 - LIST_SHORT_START + 1;
        } 

        else {
            assembly {
                let byteLen := sub(byte0, 0xf7)
                memPtr := add(memPtr, 1)

                let dataLen := div(mload(memPtr), exp(256, sub(32, byteLen))) // right shifting to the correct length
                itemLen := add(dataLen, add(byteLen, 1))
            }
        }

        return itemLen;
    }

    // @return number of bytes until the data
    function _payloadOffset(uint memPtr) private pure returns (uint) {
        uint byte0;
        assembly {
            byte0 := byte(0, mload(memPtr))
        }

        if (byte0 < STRING_SHORT_START) 
            return 0;
        else if (byte0 < STRING_LONG_START || (byte0 >= LIST_SHORT_START && byte0 < LIST_LONG_START))
            return 1;
        else if (byte0 < LIST_SHORT_START)  // being explicit
            return byte0 - (STRING_LONG_START - 1) + 1;
        else
            return byte0 - (LIST_LONG_START - 1) + 1;
    }

    /*
    * @param src Pointer to source
    * @param dest Pointer to destination
    * @param len Amount of memory to copy from the source
    */
    function copy(uint src, uint dest, uint len) private pure {
        if (len == 0) return;

        // copy as many word sizes as possible
        for (; len >= WORD_SIZE; len -= WORD_SIZE) {
            assembly {
                mstore(dest, mload(src))
            }

            src += WORD_SIZE;
            dest += WORD_SIZE;
        }

        // left over bytes. Mask is used to remove unwanted bytes from the word
        uint mask = 256 ** (WORD_SIZE - len) - 1;
        assembly {
            let srcpart := and(mload(src), not(mask)) // zero out src
            let destpart := and(mload(dest), mask) // retrieve the bytes
            mstore(dest, or(destpart, srcpart))
        }
    }
}

library WASM{
	// struct define
	
	struct info{
        uint one;
        string two;
	}
	
	struct my_array{
        int[] arr;
        address[] arr_address;
        my_message[] arr_my_message;
        int[][] arr_vec_int;
        mapping_d81e6849[][] arr_map_str;
        mapping_d81e6849[][][] arr_arr_map_str;
        tuple_87e8e7e3[] arr_tup;
	}
	
	struct my_map{
        mapping_74a97866[] mp;
        mapping_45b94d7e[] mp_my_message;
        mapping_8c77726b[] mp_vec;
        mapping_d11d5974[] mp_arr;
        mapping_ae7f40e6[] mp_arr_vect;
        mapping_ce4453c3[] mp_mp;
        mapping_7baa7fc4[] mp_tup;
	}
	
	struct my_vector{
        bytes[] vec_bytes;
        tuple_5109419f[] vec_tup;
        my_message[] vec_my_message;
        int[][] vec_array_int;
        mapping_80829b3a[][][] vect_arr_map;
        mapping_d81e6849[][] vec_map;
        mapping_8488cb18[][] vec_map_my_message;
        mapping_6475a443[][] vec_map_vec;
	}
	
	struct message{
        string head;
	}
	
	struct my_message{
        message baseclass0;
        string body;
        string end;
	}
	
	struct mapping_6475a443{
        int first;
        string[] second;
	}
	
	struct mapping_45b94d7e{
        uint first;
        my_message second;
	}
	
	struct mapping_d11d5974{
        int first;
        int[] second;
	}
	
	struct mapping_ce4453c3{
        int first;
        mapping_80829b3a[] second;
	}
	
	struct tuple_67a9771d{
        int tuple_0;
        int tuple_1;
	}
	
	struct tuple_5109419f{
        int tuple_0;
        int tuple_1;
        string tuple_2;
	}
	
	struct tuple_f097b0c4{
        int tuple_0;
        string tuple_1;
	}
	
	struct mapping_23904d07{
        int first;
        tuple_67a9771d second;
	}
	
	struct tuple_87e8e7e3{
        int tuple_0;
        tuple_f097b0c4 tuple_1;
	}
	
	struct mapping_80829b3a{
        int first;
        int second;
	}
	
	struct tuple_14bb4f0b{
        int tuple_0;
        mapping_23904d07[] tuple_1;
	}
	
	struct mapping_7baa7fc4{
        int first;
        tuple_14bb4f0b second;
	}
	
	struct mapping_8488cb18{
        uint first;
        my_message second;
	}
	
	struct mapping_d81e6849{
        int first;
        string second;
	}
	
	struct mapping_74a97866{
        uint first;
        string second;
	}
	
	struct mapping_8c77726b{
        uint first;
        int[] second;
	}
	
	struct mapping_ae7f40e6{
        int first;
        int[][] second;
	}
	

	// rlp 
	
	function encode_06271baf(info memory self) internal pure returns (bytes memory){
		bytes memory rlpPara0= RLPEncode.encodeUint(self.one);
		bytes memory rlpPara1= RLPEncode.encodeString(self.two);
		bytes[] memory allRlpPara = new bytes[](2);
		allRlpPara[0] = rlpPara0;
		allRlpPara[1] = rlpPara1;
		bytes memory payload = RLPEncode.encodeList(allRlpPara);
		return payload;
	}

	function decode_06271baf(bytes memory data) internal pure returns (info memory){
		info memory result;
		RLPReader.RLPItem memory rlpItem = RLPReader.toRlpItem(data);
		RLPReader.RLPItem[] memory allItem = RLPReader.toList(rlpItem);
		result.one = RLPReader.toUint(allItem[0]);
		result.two = RLPReader.toString(allItem[1]);
		return result;
	}

	function encode_ab530a13(message memory self) internal pure returns (bytes memory){
		bytes memory rlpPara0= RLPEncode.encodeString(self.head);
		bytes[] memory allRlpPara = new bytes[](1);
		allRlpPara[0] = rlpPara0;
		bytes memory payload = RLPEncode.encodeList(allRlpPara);
		return payload;
	}

	function decode_ab530a13(bytes memory data) internal pure returns (message memory){
		message memory result;
		RLPReader.RLPItem memory rlpItem = RLPReader.toRlpItem(data);
		RLPReader.RLPItem[] memory allItem = RLPReader.toList(rlpItem);
		result.head = RLPReader.toString(allItem[0]);
		return result;
	}

	function encode_44d3bd09(tuple_67a9771d memory self) internal pure returns (bytes memory){
		bytes memory rlpPara0= RLPEncode.encodeInt(self.tuple_0);
		bytes memory rlpPara1= RLPEncode.encodeInt(self.tuple_1);
		bytes[] memory allRlpPara = new bytes[](2);
		allRlpPara[0] = rlpPara0;
		allRlpPara[1] = rlpPara1;
		bytes memory payload = RLPEncode.encodeList(allRlpPara);
		return payload;
	}

	function decode_44d3bd09(bytes memory data) internal pure returns (tuple_67a9771d memory){
		tuple_67a9771d memory result;
		RLPReader.RLPItem memory rlpItem = RLPReader.toRlpItem(data);
		RLPReader.RLPItem[] memory allItem = RLPReader.toList(rlpItem);
		result.tuple_0 = RLPReader.toInt(allItem[0]);
		result.tuple_1 = RLPReader.toInt(allItem[1]);
		return result;
	}

	function encode_4c15b022(tuple_5109419f memory self) internal pure returns (bytes memory){
		bytes memory rlpPara0= RLPEncode.encodeInt(self.tuple_0);
		bytes memory rlpPara1= RLPEncode.encodeInt(self.tuple_1);
		bytes memory rlpPara2= RLPEncode.encodeString(self.tuple_2);
		bytes[] memory allRlpPara = new bytes[](3);
		allRlpPara[0] = rlpPara0;
		allRlpPara[1] = rlpPara1;
		allRlpPara[2] = rlpPara2;
		bytes memory payload = RLPEncode.encodeList(allRlpPara);
		return payload;
	}

	function decode_4c15b022(bytes memory data) internal pure returns (tuple_5109419f memory){
		tuple_5109419f memory result;
		RLPReader.RLPItem memory rlpItem = RLPReader.toRlpItem(data);
		RLPReader.RLPItem[] memory allItem = RLPReader.toList(rlpItem);
		result.tuple_0 = RLPReader.toInt(allItem[0]);
		result.tuple_1 = RLPReader.toInt(allItem[1]);
		result.tuple_2 = RLPReader.toString(allItem[2]);
		return result;
	}

	function encode_cee05fd9(tuple_f097b0c4 memory self) internal pure returns (bytes memory){
		bytes memory rlpPara0= RLPEncode.encodeInt(self.tuple_0);
		bytes memory rlpPara1= RLPEncode.encodeString(self.tuple_1);
		bytes[] memory allRlpPara = new bytes[](2);
		allRlpPara[0] = rlpPara0;
		allRlpPara[1] = rlpPara1;
		bytes memory payload = RLPEncode.encodeList(allRlpPara);
		return payload;
	}

	function decode_cee05fd9(bytes memory data) internal pure returns (tuple_f097b0c4 memory){
		tuple_f097b0c4 memory result;
		RLPReader.RLPItem memory rlpItem = RLPReader.toRlpItem(data);
		RLPReader.RLPItem[] memory allItem = RLPReader.toList(rlpItem);
		result.tuple_0 = RLPReader.toInt(allItem[0]);
		result.tuple_1 = RLPReader.toString(allItem[1]);
		return result;
	}

	function encode_05724597(mapping_80829b3a memory self) internal pure returns (bytes memory){
		bytes memory rlpPara0= RLPEncode.encodeInt(self.first);
		bytes memory rlpPara1= RLPEncode.encodeInt(self.second);
		bytes[] memory allRlpPara = new bytes[](2);
		allRlpPara[0] = rlpPara0;
		allRlpPara[1] = rlpPara1;
		bytes memory payload = RLPEncode.encodeList(allRlpPara);
		return payload;
	}

	function decode_05724597(bytes memory data) internal pure returns (mapping_80829b3a memory){
		mapping_80829b3a memory result;
		RLPReader.RLPItem memory rlpItem = RLPReader.toRlpItem(data);
		RLPReader.RLPItem[] memory allItem = RLPReader.toList(rlpItem);
		result.first = RLPReader.toInt(allItem[0]);
		result.second = RLPReader.toInt(allItem[1]);
		return result;
	}

	function encode_6cb67eeb(mapping_d81e6849 memory self) internal pure returns (bytes memory){
		bytes memory rlpPara0= RLPEncode.encodeInt(self.first);
		bytes memory rlpPara1= RLPEncode.encodeString(self.second);
		bytes[] memory allRlpPara = new bytes[](2);
		allRlpPara[0] = rlpPara0;
		allRlpPara[1] = rlpPara1;
		bytes memory payload = RLPEncode.encodeList(allRlpPara);
		return payload;
	}

	function decode_6cb67eeb(bytes memory data) internal pure returns (mapping_d81e6849 memory){
		mapping_d81e6849 memory result;
		RLPReader.RLPItem memory rlpItem = RLPReader.toRlpItem(data);
		RLPReader.RLPItem[] memory allItem = RLPReader.toList(rlpItem);
		result.first = RLPReader.toInt(allItem[0]);
		result.second = RLPReader.toString(allItem[1]);
		return result;
	}

	function encode_4392f745(mapping_74a97866 memory self) internal pure returns (bytes memory){
		bytes memory rlpPara0= RLPEncode.encodeUint(self.first);
		bytes memory rlpPara1= RLPEncode.encodeString(self.second);
		bytes[] memory allRlpPara = new bytes[](2);
		allRlpPara[0] = rlpPara0;
		allRlpPara[1] = rlpPara1;
		bytes memory payload = RLPEncode.encodeList(allRlpPara);
		return payload;
	}

	function decode_4392f745(bytes memory data) internal pure returns (mapping_74a97866 memory){
		mapping_74a97866 memory result;
		RLPReader.RLPItem memory rlpItem = RLPReader.toRlpItem(data);
		RLPReader.RLPItem[] memory allItem = RLPReader.toList(rlpItem);
		result.first = RLPReader.toUint(allItem[0]);
		result.second = RLPReader.toString(allItem[1]);
		return result;
	}

	function encode_8436c47c(int[] memory self) internal pure returns (bytes memory){
		uint length = self.length;
		bytes[] memory allRlpPara = new bytes[](length);
		for (uint i=0; i<length; i++) {
			allRlpPara[i] = RLPEncode.encodeInt(self[i]);
		}
		bytes memory payload = RLPEncode.encodeList(allRlpPara);
		return payload;
	}

	function decode_8436c47c(bytes memory data) internal pure returns (int[] memory){
		RLPReader.RLPItem memory rlpItem = RLPReader.toRlpItem(data);
		RLPReader.RLPItem[] memory allItem = RLPReader.toList(rlpItem);
		uint length = allItem.length;
		int[] memory result = new int[](length);
		for (uint i=0; i<length; i++) {
			result[i] = RLPReader.toInt(allItem[i]);
		}
		return result;
	}

	function encode_69724fca(address[] memory self) internal pure returns (bytes memory){
		uint length = self.length;
		bytes[] memory allRlpPara = new bytes[](length);
		for (uint i=0; i<length; i++) {
			allRlpPara[i] = RLPEncode.encodeAddress(self[i]);
		}
		bytes memory payload = RLPEncode.encodeList(allRlpPara);
		return payload;
	}

	function decode_69724fca(bytes memory data) internal pure returns (address[] memory){
		RLPReader.RLPItem memory rlpItem = RLPReader.toRlpItem(data);
		RLPReader.RLPItem[] memory allItem = RLPReader.toList(rlpItem);
		uint length = allItem.length;
		address[] memory result = new address[](length);
		for (uint i=0; i<length; i++) {
			result[i] = RLPReader.toAddress(allItem[i]);
		}
		return result;
	}

	function encode_7d0c7870(bytes[] memory self) internal pure returns (bytes memory){
		uint length = self.length;
		bytes[] memory allRlpPara = new bytes[](length);
		for (uint i=0; i<length; i++) {
			allRlpPara[i] = RLPEncode.encodeBytes(self[i]);
		}
		bytes memory payload = RLPEncode.encodeList(allRlpPara);
		return payload;
	}

	function decode_7d0c7870(bytes memory data) internal pure returns (bytes[] memory){
		RLPReader.RLPItem memory rlpItem = RLPReader.toRlpItem(data);
		RLPReader.RLPItem[] memory allItem = RLPReader.toList(rlpItem);
		uint length = allItem.length;
		bytes[] memory result = new bytes[](length);
		for (uint i=0; i<length; i++) {
			result[i] = RLPReader.toBytes(allItem[i]);
		}
		return result;
	}

	function encode_5444db16(string[] memory self) internal pure returns (bytes memory){
		uint length = self.length;
		bytes[] memory allRlpPara = new bytes[](length);
		for (uint i=0; i<length; i++) {
			allRlpPara[i] = RLPEncode.encodeString(self[i]);
		}
		bytes memory payload = RLPEncode.encodeList(allRlpPara);
		return payload;
	}

	function decode_5444db16(bytes memory data) internal pure returns (string[] memory){
		RLPReader.RLPItem memory rlpItem = RLPReader.toRlpItem(data);
		RLPReader.RLPItem[] memory allItem = RLPReader.toList(rlpItem);
		uint length = allItem.length;
		string[] memory result = new string[](length);
		for (uint i=0; i<length; i++) {
			result[i] = RLPReader.toString(allItem[i]);
		}
		return result;
	}

	function encode_114338ea(my_message memory self) internal pure returns (bytes memory){
		bytes memory rlpPara0 = encode_ab530a13(self.baseclass0);
		bytes memory rlpPara1= RLPEncode.encodeString(self.body);
		bytes memory rlpPara2= RLPEncode.encodeString(self.end);
		bytes[] memory allRlpPara = new bytes[](3);
		allRlpPara[0] = rlpPara0;
		allRlpPara[1] = rlpPara1;
		allRlpPara[2] = rlpPara2;
		bytes memory payload = RLPEncode.encodeList(allRlpPara);
		return payload;
	}

	function decode_114338ea(bytes memory data) internal pure returns (my_message memory){
		my_message memory result;
		RLPReader.RLPItem memory rlpItem = RLPReader.toRlpItem(data);
		RLPReader.RLPItem[] memory allItem = RLPReader.toList(rlpItem);
		result.baseclass0 = decode_ab530a13(RLPReader.toRlpBytes(allItem[0]));
		result.body = RLPReader.toString(allItem[1]);
		result.end = RLPReader.toString(allItem[2]);
		return result;
	}

	function encode_81f89fa1(my_message[] memory self) internal pure returns (bytes memory){
		uint length = self.length;
		bytes[] memory allRlpPara = new bytes[](length);
		for (uint i=0; i<length; i++) {
			allRlpPara[i] = encode_114338ea(self[i]);
		}
		bytes memory payload = RLPEncode.encodeList(allRlpPara);
		return payload;
	}

	function decode_81f89fa1(bytes memory data) internal pure returns (my_message[] memory){
		RLPReader.RLPItem memory rlpItem = RLPReader.toRlpItem(data);
		RLPReader.RLPItem[] memory allItem = RLPReader.toList(rlpItem);
		uint length = allItem.length;
		my_message[] memory result = new my_message[](length);
		for (uint i=0; i<length; i++) {
			result[i] = decode_114338ea(RLPReader.toRlpBytes(allItem[i]));
		}
		return result;
	}

	function encode_3712da0b(mapping_45b94d7e memory self) internal pure returns (bytes memory){
		bytes memory rlpPara0= RLPEncode.encodeUint(self.first);
		bytes memory rlpPara1 = encode_114338ea(self.second);
		bytes[] memory allRlpPara = new bytes[](2);
		allRlpPara[0] = rlpPara0;
		allRlpPara[1] = rlpPara1;
		bytes memory payload = RLPEncode.encodeList(allRlpPara);
		return payload;
	}

	function decode_3712da0b(bytes memory data) internal pure returns (mapping_45b94d7e memory){
		mapping_45b94d7e memory result;
		RLPReader.RLPItem memory rlpItem = RLPReader.toRlpItem(data);
		RLPReader.RLPItem[] memory allItem = RLPReader.toList(rlpItem);
		result.first = RLPReader.toUint(allItem[0]);
		result.second = decode_114338ea(RLPReader.toRlpBytes(allItem[1]));
		return result;
	}

	function encode_9f25bc5a(mapping_8488cb18 memory self) internal pure returns (bytes memory){
		bytes memory rlpPara0= RLPEncode.encodeUint(self.first);
		bytes memory rlpPara1 = encode_114338ea(self.second);
		bytes[] memory allRlpPara = new bytes[](2);
		allRlpPara[0] = rlpPara0;
		allRlpPara[1] = rlpPara1;
		bytes memory payload = RLPEncode.encodeList(allRlpPara);
		return payload;
	}

	function decode_9f25bc5a(bytes memory data) internal pure returns (mapping_8488cb18 memory){
		mapping_8488cb18 memory result;
		RLPReader.RLPItem memory rlpItem = RLPReader.toRlpItem(data);
		RLPReader.RLPItem[] memory allItem = RLPReader.toList(rlpItem);
		result.first = RLPReader.toUint(allItem[0]);
		result.second = decode_114338ea(RLPReader.toRlpBytes(allItem[1]));
		return result;
	}

	function encode_7c89c783(my_array memory self) internal pure returns (bytes memory){
		bytes memory rlpPara0 = encode_8436c47c(self.arr);
		bytes memory rlpPara1 = encode_69724fca(self.arr_address);
		bytes memory rlpPara2 = encode_81f89fa1(self.arr_my_message);
		bytes memory rlpPara3 = encode_384bc39f(self.arr_vec_int);
		bytes memory rlpPara4 = encode_72c85ea5(self.arr_map_str);
		bytes memory rlpPara5 = encode_2cfb22d6(self.arr_arr_map_str);
		bytes memory rlpPara6 = encode_8dad0c3f(self.arr_tup);
		bytes[] memory allRlpPara = new bytes[](7);
		allRlpPara[0] = rlpPara0;
		allRlpPara[1] = rlpPara1;
		allRlpPara[2] = rlpPara2;
		allRlpPara[3] = rlpPara3;
		allRlpPara[4] = rlpPara4;
		allRlpPara[5] = rlpPara5;
		allRlpPara[6] = rlpPara6;
		bytes memory payload = RLPEncode.encodeList(allRlpPara);
		return payload;
	}

	function decode_7c89c783(bytes memory data) internal pure returns (my_array memory){
		my_array memory result;
		RLPReader.RLPItem memory rlpItem = RLPReader.toRlpItem(data);
		RLPReader.RLPItem[] memory allItem = RLPReader.toList(rlpItem);
		result.arr = decode_8436c47c(RLPReader.toRlpBytes(allItem[0]));
		result.arr_address = decode_69724fca(RLPReader.toRlpBytes(allItem[1]));
		result.arr_my_message = decode_81f89fa1(RLPReader.toRlpBytes(allItem[2]));
		result.arr_vec_int = decode_384bc39f(RLPReader.toRlpBytes(allItem[3]));
		result.arr_map_str = decode_72c85ea5(RLPReader.toRlpBytes(allItem[4]));
		result.arr_arr_map_str = decode_2cfb22d6(RLPReader.toRlpBytes(allItem[5]));
		result.arr_tup = decode_8dad0c3f(RLPReader.toRlpBytes(allItem[6]));
		return result;
	}

	function encode_6cabc4ac(my_vector memory self) internal pure returns (bytes memory){
		bytes memory rlpPara0 = encode_7d0c7870(self.vec_bytes);
		bytes memory rlpPara1 = encode_6b76e918(self.vec_tup);
		bytes memory rlpPara2 = encode_81f89fa1(self.vec_my_message);
		bytes memory rlpPara3 = encode_384bc39f(self.vec_array_int);
		bytes memory rlpPara4 = encode_ee030222(self.vect_arr_map);
		bytes memory rlpPara5 = encode_72c85ea5(self.vec_map);
		bytes memory rlpPara6 = encode_2a879b4a(self.vec_map_my_message);
		bytes memory rlpPara7 = encode_dd891611(self.vec_map_vec);
		bytes[] memory allRlpPara = new bytes[](8);
		allRlpPara[0] = rlpPara0;
		allRlpPara[1] = rlpPara1;
		allRlpPara[2] = rlpPara2;
		allRlpPara[3] = rlpPara3;
		allRlpPara[4] = rlpPara4;
		allRlpPara[5] = rlpPara5;
		allRlpPara[6] = rlpPara6;
		allRlpPara[7] = rlpPara7;
		bytes memory payload = RLPEncode.encodeList(allRlpPara);
		return payload;
	}

	function decode_6cabc4ac(bytes memory data) internal pure returns (my_vector memory){
		my_vector memory result;
		RLPReader.RLPItem memory rlpItem = RLPReader.toRlpItem(data);
		RLPReader.RLPItem[] memory allItem = RLPReader.toList(rlpItem);
		result.vec_bytes = decode_7d0c7870(RLPReader.toRlpBytes(allItem[0]));
		result.vec_tup = decode_6b76e918(RLPReader.toRlpBytes(allItem[1]));
		result.vec_my_message = decode_81f89fa1(RLPReader.toRlpBytes(allItem[2]));
		result.vec_array_int = decode_384bc39f(RLPReader.toRlpBytes(allItem[3]));
		result.vect_arr_map = decode_ee030222(RLPReader.toRlpBytes(allItem[4]));
		result.vec_map = decode_72c85ea5(RLPReader.toRlpBytes(allItem[5]));
		result.vec_map_my_message = decode_2a879b4a(RLPReader.toRlpBytes(allItem[6]));
		result.vec_map_vec = decode_dd891611(RLPReader.toRlpBytes(allItem[7]));
		return result;
	}

	function encode_89e1dfc3(mapping_45b94d7e[] memory self) internal pure returns (bytes memory){
		uint length = self.length;
		bytes[] memory allRlpPara = new bytes[](length);
		for (uint i=0; i<length; i++) {
			allRlpPara[i] = encode_3712da0b(self[i]);
		}
		bytes memory payload = RLPEncode.encodeList(allRlpPara);
		return payload;
	}

	function decode_89e1dfc3(bytes memory data) internal pure returns (mapping_45b94d7e[] memory){
		RLPReader.RLPItem memory rlpItem = RLPReader.toRlpItem(data);
		RLPReader.RLPItem[] memory allItem = RLPReader.toList(rlpItem);
		uint length = allItem.length;
		mapping_45b94d7e[] memory result = new mapping_45b94d7e[](length);
		for (uint i=0; i<length; i++) {
			result[i] = decode_3712da0b(RLPReader.toRlpBytes(allItem[i]));
		}
		return result;
	}

	function encode_6fa6aa51(my_map memory self) internal pure returns (bytes memory){
		bytes memory rlpPara0 = encode_9337bde9(self.mp);
		bytes memory rlpPara1 = encode_89e1dfc3(self.mp_my_message);
		bytes memory rlpPara2 = encode_f856fa30(self.mp_vec);
		bytes memory rlpPara3 = encode_160eb2fc(self.mp_arr);
		bytes memory rlpPara4 = encode_b4c46907(self.mp_arr_vect);
		bytes memory rlpPara5 = encode_f707566c(self.mp_mp);
		bytes memory rlpPara6 = encode_8da1ffbe(self.mp_tup);
		bytes[] memory allRlpPara = new bytes[](7);
		allRlpPara[0] = rlpPara0;
		allRlpPara[1] = rlpPara1;
		allRlpPara[2] = rlpPara2;
		allRlpPara[3] = rlpPara3;
		allRlpPara[4] = rlpPara4;
		allRlpPara[5] = rlpPara5;
		allRlpPara[6] = rlpPara6;
		bytes memory payload = RLPEncode.encodeList(allRlpPara);
		return payload;
	}

	function decode_6fa6aa51(bytes memory data) internal pure returns (my_map memory){
		my_map memory result;
		RLPReader.RLPItem memory rlpItem = RLPReader.toRlpItem(data);
		RLPReader.RLPItem[] memory allItem = RLPReader.toList(rlpItem);
		result.mp = decode_9337bde9(RLPReader.toRlpBytes(allItem[0]));
		result.mp_my_message = decode_89e1dfc3(RLPReader.toRlpBytes(allItem[1]));
		result.mp_vec = decode_f856fa30(RLPReader.toRlpBytes(allItem[2]));
		result.mp_arr = decode_160eb2fc(RLPReader.toRlpBytes(allItem[3]));
		result.mp_arr_vect = decode_b4c46907(RLPReader.toRlpBytes(allItem[4]));
		result.mp_mp = decode_f707566c(RLPReader.toRlpBytes(allItem[5]));
		result.mp_tup = decode_8da1ffbe(RLPReader.toRlpBytes(allItem[6]));
		return result;
	}

	function encode_b80b4fc8(mapping_8488cb18[] memory self) internal pure returns (bytes memory){
		uint length = self.length;
		bytes[] memory allRlpPara = new bytes[](length);
		for (uint i=0; i<length; i++) {
			allRlpPara[i] = encode_9f25bc5a(self[i]);
		}
		bytes memory payload = RLPEncode.encodeList(allRlpPara);
		return payload;
	}

	function decode_b80b4fc8(bytes memory data) internal pure returns (mapping_8488cb18[] memory){
		RLPReader.RLPItem memory rlpItem = RLPReader.toRlpItem(data);
		RLPReader.RLPItem[] memory allItem = RLPReader.toList(rlpItem);
		uint length = allItem.length;
		mapping_8488cb18[] memory result = new mapping_8488cb18[](length);
		for (uint i=0; i<length; i++) {
			result[i] = decode_9f25bc5a(RLPReader.toRlpBytes(allItem[i]));
		}
		return result;
	}

	function encode_2a879b4a(mapping_8488cb18[][] memory self) internal pure returns (bytes memory){
		uint length = self.length;
		bytes[] memory allRlpPara = new bytes[](length);
		for (uint i=0; i<length; i++) {
			allRlpPara[i] = encode_b80b4fc8(self[i]);
		}
		bytes memory payload = RLPEncode.encodeList(allRlpPara);
		return payload;
	}

	function decode_2a879b4a(bytes memory data) internal pure returns (mapping_8488cb18[][] memory){
		RLPReader.RLPItem memory rlpItem = RLPReader.toRlpItem(data);
		RLPReader.RLPItem[] memory allItem = RLPReader.toList(rlpItem);
		uint length = allItem.length;
		mapping_8488cb18[][] memory result = new mapping_8488cb18[][](length);
		for (uint i=0; i<length; i++) {
			result[i] = decode_b80b4fc8(RLPReader.toRlpBytes(allItem[i]));
		}
		return result;
	}

	function encode_42985953(mapping_23904d07 memory self) internal pure returns (bytes memory){
		bytes memory rlpPara0= RLPEncode.encodeInt(self.first);
		bytes memory rlpPara1 = encode_44d3bd09(self.second);
		bytes[] memory allRlpPara = new bytes[](2);
		allRlpPara[0] = rlpPara0;
		allRlpPara[1] = rlpPara1;
		bytes memory payload = RLPEncode.encodeList(allRlpPara);
		return payload;
	}

	function decode_42985953(bytes memory data) internal pure returns (mapping_23904d07 memory){
		mapping_23904d07 memory result;
		RLPReader.RLPItem memory rlpItem = RLPReader.toRlpItem(data);
		RLPReader.RLPItem[] memory allItem = RLPReader.toList(rlpItem);
		result.first = RLPReader.toInt(allItem[0]);
		result.second = decode_44d3bd09(RLPReader.toRlpBytes(allItem[1]));
		return result;
	}

	function encode_9f5c2bfc(mapping_23904d07[] memory self) internal pure returns (bytes memory){
		uint length = self.length;
		bytes[] memory allRlpPara = new bytes[](length);
		for (uint i=0; i<length; i++) {
			allRlpPara[i] = encode_42985953(self[i]);
		}
		bytes memory payload = RLPEncode.encodeList(allRlpPara);
		return payload;
	}

	function decode_9f5c2bfc(bytes memory data) internal pure returns (mapping_23904d07[] memory){
		RLPReader.RLPItem memory rlpItem = RLPReader.toRlpItem(data);
		RLPReader.RLPItem[] memory allItem = RLPReader.toList(rlpItem);
		uint length = allItem.length;
		mapping_23904d07[] memory result = new mapping_23904d07[](length);
		for (uint i=0; i<length; i++) {
			result[i] = decode_42985953(RLPReader.toRlpBytes(allItem[i]));
		}
		return result;
	}

	function encode_57f07a34(tuple_14bb4f0b memory self) internal pure returns (bytes memory){
		bytes memory rlpPara0= RLPEncode.encodeInt(self.tuple_0);
		bytes memory rlpPara1 = encode_9f5c2bfc(self.tuple_1);
		bytes[] memory allRlpPara = new bytes[](2);
		allRlpPara[0] = rlpPara0;
		allRlpPara[1] = rlpPara1;
		bytes memory payload = RLPEncode.encodeList(allRlpPara);
		return payload;
	}

	function decode_57f07a34(bytes memory data) internal pure returns (tuple_14bb4f0b memory){
		tuple_14bb4f0b memory result;
		RLPReader.RLPItem memory rlpItem = RLPReader.toRlpItem(data);
		RLPReader.RLPItem[] memory allItem = RLPReader.toList(rlpItem);
		result.tuple_0 = RLPReader.toInt(allItem[0]);
		result.tuple_1 = decode_9f5c2bfc(RLPReader.toRlpBytes(allItem[1]));
		return result;
	}

	function encode_4b322d87(mapping_7baa7fc4 memory self) internal pure returns (bytes memory){
		bytes memory rlpPara0= RLPEncode.encodeInt(self.first);
		bytes memory rlpPara1 = encode_57f07a34(self.second);
		bytes[] memory allRlpPara = new bytes[](2);
		allRlpPara[0] = rlpPara0;
		allRlpPara[1] = rlpPara1;
		bytes memory payload = RLPEncode.encodeList(allRlpPara);
		return payload;
	}

	function decode_4b322d87(bytes memory data) internal pure returns (mapping_7baa7fc4 memory){
		mapping_7baa7fc4 memory result;
		RLPReader.RLPItem memory rlpItem = RLPReader.toRlpItem(data);
		RLPReader.RLPItem[] memory allItem = RLPReader.toList(rlpItem);
		result.first = RLPReader.toInt(allItem[0]);
		result.second = decode_57f07a34(RLPReader.toRlpBytes(allItem[1]));
		return result;
	}

	function encode_8da1ffbe(mapping_7baa7fc4[] memory self) internal pure returns (bytes memory){
		uint length = self.length;
		bytes[] memory allRlpPara = new bytes[](length);
		for (uint i=0; i<length; i++) {
			allRlpPara[i] = encode_4b322d87(self[i]);
		}
		bytes memory payload = RLPEncode.encodeList(allRlpPara);
		return payload;
	}

	function decode_8da1ffbe(bytes memory data) internal pure returns (mapping_7baa7fc4[] memory){
		RLPReader.RLPItem memory rlpItem = RLPReader.toRlpItem(data);
		RLPReader.RLPItem[] memory allItem = RLPReader.toList(rlpItem);
		uint length = allItem.length;
		mapping_7baa7fc4[] memory result = new mapping_7baa7fc4[](length);
		for (uint i=0; i<length; i++) {
			result[i] = decode_4b322d87(RLPReader.toRlpBytes(allItem[i]));
		}
		return result;
	}

	function encode_6b76e918(tuple_5109419f[] memory self) internal pure returns (bytes memory){
		uint length = self.length;
		bytes[] memory allRlpPara = new bytes[](length);
		for (uint i=0; i<length; i++) {
			allRlpPara[i] = encode_4c15b022(self[i]);
		}
		bytes memory payload = RLPEncode.encodeList(allRlpPara);
		return payload;
	}

	function decode_6b76e918(bytes memory data) internal pure returns (tuple_5109419f[] memory){
		RLPReader.RLPItem memory rlpItem = RLPReader.toRlpItem(data);
		RLPReader.RLPItem[] memory allItem = RLPReader.toList(rlpItem);
		uint length = allItem.length;
		tuple_5109419f[] memory result = new tuple_5109419f[](length);
		for (uint i=0; i<length; i++) {
			result[i] = decode_4c15b022(RLPReader.toRlpBytes(allItem[i]));
		}
		return result;
	}

	function encode_d1e99841(tuple_87e8e7e3 memory self) internal pure returns (bytes memory){
		bytes memory rlpPara0= RLPEncode.encodeInt(self.tuple_0);
		bytes memory rlpPara1 = encode_cee05fd9(self.tuple_1);
		bytes[] memory allRlpPara = new bytes[](2);
		allRlpPara[0] = rlpPara0;
		allRlpPara[1] = rlpPara1;
		bytes memory payload = RLPEncode.encodeList(allRlpPara);
		return payload;
	}

	function decode_d1e99841(bytes memory data) internal pure returns (tuple_87e8e7e3 memory){
		tuple_87e8e7e3 memory result;
		RLPReader.RLPItem memory rlpItem = RLPReader.toRlpItem(data);
		RLPReader.RLPItem[] memory allItem = RLPReader.toList(rlpItem);
		result.tuple_0 = RLPReader.toInt(allItem[0]);
		result.tuple_1 = decode_cee05fd9(RLPReader.toRlpBytes(allItem[1]));
		return result;
	}

	function encode_8dad0c3f(tuple_87e8e7e3[] memory self) internal pure returns (bytes memory){
		uint length = self.length;
		bytes[] memory allRlpPara = new bytes[](length);
		for (uint i=0; i<length; i++) {
			allRlpPara[i] = encode_d1e99841(self[i]);
		}
		bytes memory payload = RLPEncode.encodeList(allRlpPara);
		return payload;
	}

	function decode_8dad0c3f(bytes memory data) internal pure returns (tuple_87e8e7e3[] memory){
		RLPReader.RLPItem memory rlpItem = RLPReader.toRlpItem(data);
		RLPReader.RLPItem[] memory allItem = RLPReader.toList(rlpItem);
		uint length = allItem.length;
		tuple_87e8e7e3[] memory result = new tuple_87e8e7e3[](length);
		for (uint i=0; i<length; i++) {
			result[i] = decode_d1e99841(RLPReader.toRlpBytes(allItem[i]));
		}
		return result;
	}

	function encode_338f5315(mapping_80829b3a[] memory self) internal pure returns (bytes memory){
		uint length = self.length;
		bytes[] memory allRlpPara = new bytes[](length);
		for (uint i=0; i<length; i++) {
			allRlpPara[i] = encode_05724597(self[i]);
		}
		bytes memory payload = RLPEncode.encodeList(allRlpPara);
		return payload;
	}

	function decode_338f5315(bytes memory data) internal pure returns (mapping_80829b3a[] memory){
		RLPReader.RLPItem memory rlpItem = RLPReader.toRlpItem(data);
		RLPReader.RLPItem[] memory allItem = RLPReader.toList(rlpItem);
		uint length = allItem.length;
		mapping_80829b3a[] memory result = new mapping_80829b3a[](length);
		for (uint i=0; i<length; i++) {
			result[i] = decode_05724597(RLPReader.toRlpBytes(allItem[i]));
		}
		return result;
	}

	function encode_c90aab10(mapping_80829b3a[][] memory self) internal pure returns (bytes memory){
		uint length = self.length;
		bytes[] memory allRlpPara = new bytes[](length);
		for (uint i=0; i<length; i++) {
			allRlpPara[i] = encode_338f5315(self[i]);
		}
		bytes memory payload = RLPEncode.encodeList(allRlpPara);
		return payload;
	}

	function decode_c90aab10(bytes memory data) internal pure returns (mapping_80829b3a[][] memory){
		RLPReader.RLPItem memory rlpItem = RLPReader.toRlpItem(data);
		RLPReader.RLPItem[] memory allItem = RLPReader.toList(rlpItem);
		uint length = allItem.length;
		mapping_80829b3a[][] memory result = new mapping_80829b3a[][](length);
		for (uint i=0; i<length; i++) {
			result[i] = decode_338f5315(RLPReader.toRlpBytes(allItem[i]));
		}
		return result;
	}

	function encode_30a5938f(mapping_ce4453c3 memory self) internal pure returns (bytes memory){
		bytes memory rlpPara0= RLPEncode.encodeInt(self.first);
		bytes memory rlpPara1 = encode_338f5315(self.second);
		bytes[] memory allRlpPara = new bytes[](2);
		allRlpPara[0] = rlpPara0;
		allRlpPara[1] = rlpPara1;
		bytes memory payload = RLPEncode.encodeList(allRlpPara);
		return payload;
	}

	function decode_30a5938f(bytes memory data) internal pure returns (mapping_ce4453c3 memory){
		mapping_ce4453c3 memory result;
		RLPReader.RLPItem memory rlpItem = RLPReader.toRlpItem(data);
		RLPReader.RLPItem[] memory allItem = RLPReader.toList(rlpItem);
		result.first = RLPReader.toInt(allItem[0]);
		result.second = decode_338f5315(RLPReader.toRlpBytes(allItem[1]));
		return result;
	}

	function encode_ee030222(mapping_80829b3a[][][] memory self) internal pure returns (bytes memory){
		uint length = self.length;
		bytes[] memory allRlpPara = new bytes[](length);
		for (uint i=0; i<length; i++) {
			allRlpPara[i] = encode_c90aab10(self[i]);
		}
		bytes memory payload = RLPEncode.encodeList(allRlpPara);
		return payload;
	}

	function decode_ee030222(bytes memory data) internal pure returns (mapping_80829b3a[][][] memory){
		RLPReader.RLPItem memory rlpItem = RLPReader.toRlpItem(data);
		RLPReader.RLPItem[] memory allItem = RLPReader.toList(rlpItem);
		uint length = allItem.length;
		mapping_80829b3a[][][] memory result = new mapping_80829b3a[][][](length);
		for (uint i=0; i<length; i++) {
			result[i] = decode_c90aab10(RLPReader.toRlpBytes(allItem[i]));
		}
		return result;
	}

	function encode_f707566c(mapping_ce4453c3[] memory self) internal pure returns (bytes memory){
		uint length = self.length;
		bytes[] memory allRlpPara = new bytes[](length);
		for (uint i=0; i<length; i++) {
			allRlpPara[i] = encode_30a5938f(self[i]);
		}
		bytes memory payload = RLPEncode.encodeList(allRlpPara);
		return payload;
	}

	function decode_f707566c(bytes memory data) internal pure returns (mapping_ce4453c3[] memory){
		RLPReader.RLPItem memory rlpItem = RLPReader.toRlpItem(data);
		RLPReader.RLPItem[] memory allItem = RLPReader.toList(rlpItem);
		uint length = allItem.length;
		mapping_ce4453c3[] memory result = new mapping_ce4453c3[](length);
		for (uint i=0; i<length; i++) {
			result[i] = decode_30a5938f(RLPReader.toRlpBytes(allItem[i]));
		}
		return result;
	}

	function encode_e86c1526(mapping_d81e6849[] memory self) internal pure returns (bytes memory){
		uint length = self.length;
		bytes[] memory allRlpPara = new bytes[](length);
		for (uint i=0; i<length; i++) {
			allRlpPara[i] = encode_6cb67eeb(self[i]);
		}
		bytes memory payload = RLPEncode.encodeList(allRlpPara);
		return payload;
	}

	function decode_e86c1526(bytes memory data) internal pure returns (mapping_d81e6849[] memory){
		RLPReader.RLPItem memory rlpItem = RLPReader.toRlpItem(data);
		RLPReader.RLPItem[] memory allItem = RLPReader.toList(rlpItem);
		uint length = allItem.length;
		mapping_d81e6849[] memory result = new mapping_d81e6849[](length);
		for (uint i=0; i<length; i++) {
			result[i] = decode_6cb67eeb(RLPReader.toRlpBytes(allItem[i]));
		}
		return result;
	}

	function encode_72c85ea5(mapping_d81e6849[][] memory self) internal pure returns (bytes memory){
		uint length = self.length;
		bytes[] memory allRlpPara = new bytes[](length);
		for (uint i=0; i<length; i++) {
			allRlpPara[i] = encode_e86c1526(self[i]);
		}
		bytes memory payload = RLPEncode.encodeList(allRlpPara);
		return payload;
	}

	function decode_72c85ea5(bytes memory data) internal pure returns (mapping_d81e6849[][] memory){
		RLPReader.RLPItem memory rlpItem = RLPReader.toRlpItem(data);
		RLPReader.RLPItem[] memory allItem = RLPReader.toList(rlpItem);
		uint length = allItem.length;
		mapping_d81e6849[][] memory result = new mapping_d81e6849[][](length);
		for (uint i=0; i<length; i++) {
			result[i] = decode_e86c1526(RLPReader.toRlpBytes(allItem[i]));
		}
		return result;
	}

	function encode_2cfb22d6(mapping_d81e6849[][][] memory self) internal pure returns (bytes memory){
		uint length = self.length;
		bytes[] memory allRlpPara = new bytes[](length);
		for (uint i=0; i<length; i++) {
			allRlpPara[i] = encode_72c85ea5(self[i]);
		}
		bytes memory payload = RLPEncode.encodeList(allRlpPara);
		return payload;
	}

	function decode_2cfb22d6(bytes memory data) internal pure returns (mapping_d81e6849[][][] memory){
		RLPReader.RLPItem memory rlpItem = RLPReader.toRlpItem(data);
		RLPReader.RLPItem[] memory allItem = RLPReader.toList(rlpItem);
		uint length = allItem.length;
		mapping_d81e6849[][][] memory result = new mapping_d81e6849[][][](length);
		for (uint i=0; i<length; i++) {
			result[i] = decode_72c85ea5(RLPReader.toRlpBytes(allItem[i]));
		}
		return result;
	}

	function encode_9337bde9(mapping_74a97866[] memory self) internal pure returns (bytes memory){
		uint length = self.length;
		bytes[] memory allRlpPara = new bytes[](length);
		for (uint i=0; i<length; i++) {
			allRlpPara[i] = encode_4392f745(self[i]);
		}
		bytes memory payload = RLPEncode.encodeList(allRlpPara);
		return payload;
	}

	function decode_9337bde9(bytes memory data) internal pure returns (mapping_74a97866[] memory){
		RLPReader.RLPItem memory rlpItem = RLPReader.toRlpItem(data);
		RLPReader.RLPItem[] memory allItem = RLPReader.toList(rlpItem);
		uint length = allItem.length;
		mapping_74a97866[] memory result = new mapping_74a97866[](length);
		for (uint i=0; i<length; i++) {
			result[i] = decode_4392f745(RLPReader.toRlpBytes(allItem[i]));
		}
		return result;
	}

	function encode_384bc39f(int[][] memory self) internal pure returns (bytes memory){
		uint length = self.length;
		bytes[] memory allRlpPara = new bytes[](length);
		for (uint i=0; i<length; i++) {
			allRlpPara[i] = encode_8436c47c(self[i]);
		}
		bytes memory payload = RLPEncode.encodeList(allRlpPara);
		return payload;
	}

	function decode_384bc39f(bytes memory data) internal pure returns (int[][] memory){
		RLPReader.RLPItem memory rlpItem = RLPReader.toRlpItem(data);
		RLPReader.RLPItem[] memory allItem = RLPReader.toList(rlpItem);
		uint length = allItem.length;
		int[][] memory result = new int[][](length);
		for (uint i=0; i<length; i++) {
			result[i] = decode_8436c47c(RLPReader.toRlpBytes(allItem[i]));
		}
		return result;
	}

	function encode_3122a04e(mapping_d11d5974 memory self) internal pure returns (bytes memory){
		bytes memory rlpPara0= RLPEncode.encodeInt(self.first);
		bytes memory rlpPara1 = encode_8436c47c(self.second);
		bytes[] memory allRlpPara = new bytes[](2);
		allRlpPara[0] = rlpPara0;
		allRlpPara[1] = rlpPara1;
		bytes memory payload = RLPEncode.encodeList(allRlpPara);
		return payload;
	}

	function decode_3122a04e(bytes memory data) internal pure returns (mapping_d11d5974 memory){
		mapping_d11d5974 memory result;
		RLPReader.RLPItem memory rlpItem = RLPReader.toRlpItem(data);
		RLPReader.RLPItem[] memory allItem = RLPReader.toList(rlpItem);
		result.first = RLPReader.toInt(allItem[0]);
		result.second = decode_8436c47c(RLPReader.toRlpBytes(allItem[1]));
		return result;
	}

	function encode_83b07978(mapping_8c77726b memory self) internal pure returns (bytes memory){
		bytes memory rlpPara0= RLPEncode.encodeUint(self.first);
		bytes memory rlpPara1 = encode_8436c47c(self.second);
		bytes[] memory allRlpPara = new bytes[](2);
		allRlpPara[0] = rlpPara0;
		allRlpPara[1] = rlpPara1;
		bytes memory payload = RLPEncode.encodeList(allRlpPara);
		return payload;
	}

	function decode_83b07978(bytes memory data) internal pure returns (mapping_8c77726b memory){
		mapping_8c77726b memory result;
		RLPReader.RLPItem memory rlpItem = RLPReader.toRlpItem(data);
		RLPReader.RLPItem[] memory allItem = RLPReader.toList(rlpItem);
		result.first = RLPReader.toUint(allItem[0]);
		result.second = decode_8436c47c(RLPReader.toRlpBytes(allItem[1]));
		return result;
	}

	function encode_b22d2264(mapping_ae7f40e6 memory self) internal pure returns (bytes memory){
		bytes memory rlpPara0= RLPEncode.encodeInt(self.first);
		bytes memory rlpPara1 = encode_384bc39f(self.second);
		bytes[] memory allRlpPara = new bytes[](2);
		allRlpPara[0] = rlpPara0;
		allRlpPara[1] = rlpPara1;
		bytes memory payload = RLPEncode.encodeList(allRlpPara);
		return payload;
	}

	function decode_b22d2264(bytes memory data) internal pure returns (mapping_ae7f40e6 memory){
		mapping_ae7f40e6 memory result;
		RLPReader.RLPItem memory rlpItem = RLPReader.toRlpItem(data);
		RLPReader.RLPItem[] memory allItem = RLPReader.toList(rlpItem);
		result.first = RLPReader.toInt(allItem[0]);
		result.second = decode_384bc39f(RLPReader.toRlpBytes(allItem[1]));
		return result;
	}

	function encode_b4c46907(mapping_ae7f40e6[] memory self) internal pure returns (bytes memory){
		uint length = self.length;
		bytes[] memory allRlpPara = new bytes[](length);
		for (uint i=0; i<length; i++) {
			allRlpPara[i] = encode_b22d2264(self[i]);
		}
		bytes memory payload = RLPEncode.encodeList(allRlpPara);
		return payload;
	}

	function decode_b4c46907(bytes memory data) internal pure returns (mapping_ae7f40e6[] memory){
		RLPReader.RLPItem memory rlpItem = RLPReader.toRlpItem(data);
		RLPReader.RLPItem[] memory allItem = RLPReader.toList(rlpItem);
		uint length = allItem.length;
		mapping_ae7f40e6[] memory result = new mapping_ae7f40e6[](length);
		for (uint i=0; i<length; i++) {
			result[i] = decode_b22d2264(RLPReader.toRlpBytes(allItem[i]));
		}
		return result;
	}

	function encode_160eb2fc(mapping_d11d5974[] memory self) internal pure returns (bytes memory){
		uint length = self.length;
		bytes[] memory allRlpPara = new bytes[](length);
		for (uint i=0; i<length; i++) {
			allRlpPara[i] = encode_3122a04e(self[i]);
		}
		bytes memory payload = RLPEncode.encodeList(allRlpPara);
		return payload;
	}

	function decode_160eb2fc(bytes memory data) internal pure returns (mapping_d11d5974[] memory){
		RLPReader.RLPItem memory rlpItem = RLPReader.toRlpItem(data);
		RLPReader.RLPItem[] memory allItem = RLPReader.toList(rlpItem);
		uint length = allItem.length;
		mapping_d11d5974[] memory result = new mapping_d11d5974[](length);
		for (uint i=0; i<length; i++) {
			result[i] = decode_3122a04e(RLPReader.toRlpBytes(allItem[i]));
		}
		return result;
	}

	function encode_f856fa30(mapping_8c77726b[] memory self) internal pure returns (bytes memory){
		uint length = self.length;
		bytes[] memory allRlpPara = new bytes[](length);
		for (uint i=0; i<length; i++) {
			allRlpPara[i] = encode_83b07978(self[i]);
		}
		bytes memory payload = RLPEncode.encodeList(allRlpPara);
		return payload;
	}

	function decode_f856fa30(bytes memory data) internal pure returns (mapping_8c77726b[] memory){
		RLPReader.RLPItem memory rlpItem = RLPReader.toRlpItem(data);
		RLPReader.RLPItem[] memory allItem = RLPReader.toList(rlpItem);
		uint length = allItem.length;
		mapping_8c77726b[] memory result = new mapping_8c77726b[](length);
		for (uint i=0; i<length; i++) {
			result[i] = decode_83b07978(RLPReader.toRlpBytes(allItem[i]));
		}
		return result;
	}

	function encode_918178ec(mapping_6475a443 memory self) internal pure returns (bytes memory){
		bytes memory rlpPara0= RLPEncode.encodeInt(self.first);
		bytes memory rlpPara1 = encode_5444db16(self.second);
		bytes[] memory allRlpPara = new bytes[](2);
		allRlpPara[0] = rlpPara0;
		allRlpPara[1] = rlpPara1;
		bytes memory payload = RLPEncode.encodeList(allRlpPara);
		return payload;
	}

	function decode_918178ec(bytes memory data) internal pure returns (mapping_6475a443 memory){
		mapping_6475a443 memory result;
		RLPReader.RLPItem memory rlpItem = RLPReader.toRlpItem(data);
		RLPReader.RLPItem[] memory allItem = RLPReader.toList(rlpItem);
		result.first = RLPReader.toInt(allItem[0]);
		result.second = decode_5444db16(RLPReader.toRlpBytes(allItem[1]));
		return result;
	}

	function encode_ef3ca2da(mapping_6475a443[] memory self) internal pure returns (bytes memory){
		uint length = self.length;
		bytes[] memory allRlpPara = new bytes[](length);
		for (uint i=0; i<length; i++) {
			allRlpPara[i] = encode_918178ec(self[i]);
		}
		bytes memory payload = RLPEncode.encodeList(allRlpPara);
		return payload;
	}

	function decode_ef3ca2da(bytes memory data) internal pure returns (mapping_6475a443[] memory){
		RLPReader.RLPItem memory rlpItem = RLPReader.toRlpItem(data);
		RLPReader.RLPItem[] memory allItem = RLPReader.toList(rlpItem);
		uint length = allItem.length;
		mapping_6475a443[] memory result = new mapping_6475a443[](length);
		for (uint i=0; i<length; i++) {
			result[i] = decode_918178ec(RLPReader.toRlpBytes(allItem[i]));
		}
		return result;
	}

	function encode_dd891611(mapping_6475a443[][] memory self) internal pure returns (bytes memory){
		uint length = self.length;
		bytes[] memory allRlpPara = new bytes[](length);
		for (uint i=0; i<length; i++) {
			allRlpPara[i] = encode_ef3ca2da(self[i]);
		}
		bytes memory payload = RLPEncode.encodeList(allRlpPara);
		return payload;
	}

	function decode_dd891611(bytes memory data) internal pure returns (mapping_6475a443[][] memory){
		RLPReader.RLPItem memory rlpItem = RLPReader.toRlpItem(data);
		RLPReader.RLPItem[] memory allItem = RLPReader.toList(rlpItem);
		uint length = allItem.length;
		mapping_6475a443[][] memory result = new mapping_6475a443[][](length);
		for (uint i=0; i<length; i++) {
			result[i] = decode_ef3ca2da(RLPReader.toRlpBytes(allItem[i]));
		}
		return result;
	}


	// call funciton define

    function set_my_vector_call(address wasm, my_vector memory one) internal {
        uint64 methord = RLPEncode.hash("set_my_vector");
        bytes memory rlpMethord = RLPEncode.encodeUint(uint(methord));
        
		bytes memory rlpPara0 = encode_6cabc4ac(one);
		bytes[] memory allInput = new bytes[](2);
		allInput[0] = rlpMethord;
		allInput[1] = rlpPara0;
		bytes memory payload = RLPEncode.encodeList(allInput);
        (bool success,) = wasm.call(payload);
        if(!success){
            revert();
        }
    }

    function set_my_map_call(address wasm, my_map memory one) internal {
        uint64 methord = RLPEncode.hash("set_my_map");
        bytes memory rlpMethord = RLPEncode.encodeUint(uint(methord));
        
		bytes memory rlpPara0 = encode_6fa6aa51(one);
		bytes[] memory allInput = new bytes[](2);
		allInput[0] = rlpMethord;
		allInput[1] = rlpPara0;
		bytes memory payload = RLPEncode.encodeList(allInput);
        (bool success,) = wasm.call(payload);
        if(!success){
            revert();
        }
    }
    
	function get_my_map_call(address wasm) internal returns (my_map memory){
		uint64 methord = RLPEncode.hash("get_my_map");
		bytes memory rlpMethord = RLPEncode.encodeUint(uint(methord));
		
		bytes[] memory allInput = new bytes[](1);
		allInput[0] = rlpMethord;
		bytes memory payload = RLPEncode.encodeList(allInput);
		(bool success, bytes memory data) = wasm.call(payload);
		if(!success){
			revert();
		}
		
		my_map memory result  = decode_6fa6aa51(data);
		return result;
	}

    function set_address_call(address wasm, address input) internal {
        uint64 methord = RLPEncode.hash("set_address");
        bytes memory rlpMethord = RLPEncode.encodeUint(uint(methord));
        
		bytes memory rlpPara0 = RLPEncode.encodeAddress(input);
		bytes[] memory allInput = new bytes[](2);
		allInput[0] = rlpMethord;
		allInput[1] = rlpPara0;
		bytes memory payload = RLPEncode.encodeList(allInput);
        (bool success,) = wasm.call(payload);
        if(!success){
            revert();
        }
    }
    
	function get_my_vector_call(address wasm) internal returns (my_vector memory){
		uint64 methord = RLPEncode.hash("get_my_vector");
		bytes memory rlpMethord = RLPEncode.encodeUint(uint(methord));
		
		bytes[] memory allInput = new bytes[](1);
		allInput[0] = rlpMethord;
		bytes memory payload = RLPEncode.encodeList(allInput);
		(bool success, bytes memory data) = wasm.call(payload);
		if(!success){
			revert();
		}
		
		my_vector memory result  = decode_6cabc4ac(data);
		return result;
	}
    
	function get_info_call(address wasm) internal returns (info memory){
		uint64 methord = RLPEncode.hash("get_info");
		bytes memory rlpMethord = RLPEncode.encodeUint(uint(methord));
		
		bytes[] memory allInput = new bytes[](1);
		allInput[0] = rlpMethord;
		bytes memory payload = RLPEncode.encodeList(allInput);
		(bool success, bytes memory data) = wasm.call(payload);
		if(!success){
			revert();
		}
		
		info memory result  = decode_06271baf(data);
		return result;
	}

    function set_uint_call(address wasm, uint input) internal {
        uint64 methord = RLPEncode.hash("set_uint");
        bytes memory rlpMethord = RLPEncode.encodeUint(uint(methord));
        
		bytes memory rlpPara0 = RLPEncode.encodeUint(input);
		bytes[] memory allInput = new bytes[](2);
		allInput[0] = rlpMethord;
		allInput[1] = rlpPara0;
		bytes memory payload = RLPEncode.encodeList(allInput);
        (bool success,) = wasm.call(payload);
        if(!success){
            revert();
        }
    }

    function set_int_call(address wasm, int input) internal {
        uint64 methord = RLPEncode.hash("set_int");
        bytes memory rlpMethord = RLPEncode.encodeUint(uint(methord));
        
		bytes memory rlpPara0 = RLPEncode.encodeInt(input);
		bytes[] memory allInput = new bytes[](2);
		allInput[0] = rlpMethord;
		allInput[1] = rlpPara0;
		bytes memory payload = RLPEncode.encodeList(allInput);
        (bool success,) = wasm.call(payload);
        if(!success){
            revert();
        }
    }
    
	function get_address_call(address wasm) internal returns (address){
		uint64 methord = RLPEncode.hash("get_address");
		bytes memory rlpMethord = RLPEncode.encodeUint(uint(methord));
		
		bytes[] memory allInput = new bytes[](1);
		allInput[0] = rlpMethord;
		bytes memory payload = RLPEncode.encodeList(allInput);
		(bool success, bytes memory data) = wasm.call(payload);
		if(!success){
			revert();
		}
		
		RLPReader.RLPItem memory rlpItem = RLPReader.toRlpItem(data);
		address result  = RLPReader.toAddress(rlpItem);
		return result;
	}
    
	function get_message_call(address wasm) internal returns (my_message[] memory){
		uint64 methord = RLPEncode.hash("get_message");
		bytes memory rlpMethord = RLPEncode.encodeUint(uint(methord));
		
		bytes[] memory allInput = new bytes[](1);
		allInput[0] = rlpMethord;
		bytes memory payload = RLPEncode.encodeList(allInput);
		(bool success, bytes memory data) = wasm.call(payload);
		if(!success){
			revert();
		}
		
		my_message[] memory result  = decode_81f89fa1(data);
		return result;
	}

    function set_my_array_call(address wasm, my_array memory one) internal {
        uint64 methord = RLPEncode.hash("set_my_array");
        bytes memory rlpMethord = RLPEncode.encodeUint(uint(methord));
        
		bytes memory rlpPara0 = encode_7c89c783(one);
		bytes[] memory allInput = new bytes[](2);
		allInput[0] = rlpMethord;
		allInput[1] = rlpPara0;
		bytes memory payload = RLPEncode.encodeList(allInput);
        (bool success,) = wasm.call(payload);
        if(!success){
            revert();
        }
    }
    
	function get_my_array_call(address wasm) internal returns (my_array memory){
		uint64 methord = RLPEncode.hash("get_my_array");
		bytes memory rlpMethord = RLPEncode.encodeUint(uint(methord));
		
		bytes[] memory allInput = new bytes[](1);
		allInput[0] = rlpMethord;
		bytes memory payload = RLPEncode.encodeList(allInput);
		(bool success, bytes memory data) = wasm.call(payload);
		if(!success){
			revert();
		}
		
		my_array memory result  = decode_7c89c783(data);
		return result;
	}
    
	function get_uint_call(address wasm) internal returns (uint){
		uint64 methord = RLPEncode.hash("get_uint");
		bytes memory rlpMethord = RLPEncode.encodeUint(uint(methord));
		
		bytes[] memory allInput = new bytes[](1);
		allInput[0] = rlpMethord;
		bytes memory payload = RLPEncode.encodeList(allInput);
		(bool success, bytes memory data) = wasm.call(payload);
		if(!success){
			revert();
		}
		
		RLPReader.RLPItem memory rlpItem = RLPReader.toRlpItem(data);
		uint result  = RLPReader.toUint(rlpItem);
		return result;
	}
    
	function get_int_call(address wasm) internal returns (int){
		uint64 methord = RLPEncode.hash("get_int");
		bytes memory rlpMethord = RLPEncode.encodeUint(uint(methord));
		
		bytes[] memory allInput = new bytes[](1);
		allInput[0] = rlpMethord;
		bytes memory payload = RLPEncode.encodeList(allInput);
		(bool success, bytes memory data) = wasm.call(payload);
		if(!success){
			revert();
		}
		
		RLPReader.RLPItem memory rlpItem = RLPReader.toRlpItem(data);
		int result  = RLPReader.toInt(rlpItem);
		return result;
	}
    
	function get_string_call(address wasm) internal returns (string memory){
		uint64 methord = RLPEncode.hash("get_string");
		bytes memory rlpMethord = RLPEncode.encodeUint(uint(methord));
		
		bytes[] memory allInput = new bytes[](1);
		allInput[0] = rlpMethord;
		bytes memory payload = RLPEncode.encodeList(allInput);
		(bool success, bytes memory data) = wasm.call(payload);
		if(!success){
			revert();
		}
		
		RLPReader.RLPItem memory rlpItem = RLPReader.toRlpItem(data);
		string memory result  = RLPReader.toString(rlpItem);
		return result;
	}

    function add_message_call(address wasm, my_message memory one_message) internal {
        uint64 methord = RLPEncode.hash("add_message");
        bytes memory rlpMethord = RLPEncode.encodeUint(uint(methord));
        
		bytes memory rlpPara0 = encode_114338ea(one_message);
		bytes[] memory allInput = new bytes[](2);
		allInput[0] = rlpMethord;
		allInput[1] = rlpPara0;
		bytes memory payload = RLPEncode.encodeList(allInput);
        (bool success,) = wasm.call(payload);
        if(!success){
            revert();
        }
    }

    function set_string_call(address wasm, string memory input) internal {
        uint64 methord = RLPEncode.hash("set_string");
        bytes memory rlpMethord = RLPEncode.encodeUint(uint(methord));
        
		bytes memory rlpPara0 = RLPEncode.encodeString(input);
		bytes[] memory allInput = new bytes[](2);
		allInput[0] = rlpMethord;
		allInput[1] = rlpPara0;
		bytes memory payload = RLPEncode.encodeList(allInput);
        (bool success,) = wasm.call(payload);
        if(!success){
            revert();
        }
    }

    function set_info_call(address wasm, info memory input) internal {
        uint64 methord = RLPEncode.hash("set_info");
        bytes memory rlpMethord = RLPEncode.encodeUint(uint(methord));
        
		bytes memory rlpPara0 = encode_06271baf(input);
		bytes[] memory allInput = new bytes[](2);
		allInput[0] = rlpMethord;
		allInput[1] = rlpPara0;
		bytes memory payload = RLPEncode.encodeList(allInput);
        (bool success,) = wasm.call(payload);
        if(!success){
            revert();
        }
    }

	// delegate call function define

    function set_my_vector_delegatecall(address wasm, my_vector memory one) internal {
        uint64 methord = RLPEncode.hash("set_my_vector");
        bytes memory rlpMethord = RLPEncode.encodeUint(uint(methord));
        
		bytes memory rlpPara0 = encode_6cabc4ac(one);
		bytes[] memory allInput = new bytes[](2);
		allInput[0] = rlpMethord;
		allInput[1] = rlpPara0;
		bytes memory payload = RLPEncode.encodeList(allInput);
        (bool success,) = wasm.delegatecall(payload);
        if(!success){
            revert();
        }
    }

    function set_my_map_delegatecall(address wasm, my_map memory one) internal {
        uint64 methord = RLPEncode.hash("set_my_map");
        bytes memory rlpMethord = RLPEncode.encodeUint(uint(methord));
        
		bytes memory rlpPara0 = encode_6fa6aa51(one);
		bytes[] memory allInput = new bytes[](2);
		allInput[0] = rlpMethord;
		allInput[1] = rlpPara0;
		bytes memory payload = RLPEncode.encodeList(allInput);
        (bool success,) = wasm.delegatecall(payload);
        if(!success){
            revert();
        }
    }
    
	function get_my_map_delegatecall(address wasm) internal returns (my_map memory){
		uint64 methord = RLPEncode.hash("get_my_map");
		bytes memory rlpMethord = RLPEncode.encodeUint(uint(methord));
		
		bytes[] memory allInput = new bytes[](1);
		allInput[0] = rlpMethord;
		bytes memory payload = RLPEncode.encodeList(allInput);
		(bool success, bytes memory data) = wasm.delegatecall(payload);
		if(!success){
			revert();
		}
		
		my_map memory result  = decode_6fa6aa51(data);
		return result;
	}

    function set_address_delegatecall(address wasm, address input) internal {
        uint64 methord = RLPEncode.hash("set_address");
        bytes memory rlpMethord = RLPEncode.encodeUint(uint(methord));
        
		bytes memory rlpPara0 = RLPEncode.encodeAddress(input);
		bytes[] memory allInput = new bytes[](2);
		allInput[0] = rlpMethord;
		allInput[1] = rlpPara0;
		bytes memory payload = RLPEncode.encodeList(allInput);
        (bool success,) = wasm.delegatecall(payload);
        if(!success){
            revert();
        }
    }
    
	function get_my_vector_delegatecall(address wasm) internal returns (my_vector memory){
		uint64 methord = RLPEncode.hash("get_my_vector");
		bytes memory rlpMethord = RLPEncode.encodeUint(uint(methord));
		
		bytes[] memory allInput = new bytes[](1);
		allInput[0] = rlpMethord;
		bytes memory payload = RLPEncode.encodeList(allInput);
		(bool success, bytes memory data) = wasm.delegatecall(payload);
		if(!success){
			revert();
		}
		
		my_vector memory result  = decode_6cabc4ac(data);
		return result;
	}
    
	function get_info_delegatecall(address wasm) internal returns (info memory){
		uint64 methord = RLPEncode.hash("get_info");
		bytes memory rlpMethord = RLPEncode.encodeUint(uint(methord));
		
		bytes[] memory allInput = new bytes[](1);
		allInput[0] = rlpMethord;
		bytes memory payload = RLPEncode.encodeList(allInput);
		(bool success, bytes memory data) = wasm.delegatecall(payload);
		if(!success){
			revert();
		}
		
		info memory result  = decode_06271baf(data);
		return result;
	}

    function set_uint_delegatecall(address wasm, uint input) internal {
        uint64 methord = RLPEncode.hash("set_uint");
        bytes memory rlpMethord = RLPEncode.encodeUint(uint(methord));
        
		bytes memory rlpPara0 = RLPEncode.encodeUint(input);
		bytes[] memory allInput = new bytes[](2);
		allInput[0] = rlpMethord;
		allInput[1] = rlpPara0;
		bytes memory payload = RLPEncode.encodeList(allInput);
        (bool success,) = wasm.delegatecall(payload);
        if(!success){
            revert();
        }
    }

    function set_int_delegatecall(address wasm, int input) internal {
        uint64 methord = RLPEncode.hash("set_int");
        bytes memory rlpMethord = RLPEncode.encodeUint(uint(methord));
        
		bytes memory rlpPara0 = RLPEncode.encodeInt(input);
		bytes[] memory allInput = new bytes[](2);
		allInput[0] = rlpMethord;
		allInput[1] = rlpPara0;
		bytes memory payload = RLPEncode.encodeList(allInput);
        (bool success,) = wasm.delegatecall(payload);
        if(!success){
            revert();
        }
    }
    
	function get_address_delegatecall(address wasm) internal returns (address){
		uint64 methord = RLPEncode.hash("get_address");
		bytes memory rlpMethord = RLPEncode.encodeUint(uint(methord));
		
		bytes[] memory allInput = new bytes[](1);
		allInput[0] = rlpMethord;
		bytes memory payload = RLPEncode.encodeList(allInput);
		(bool success, bytes memory data) = wasm.delegatecall(payload);
		if(!success){
			revert();
		}
		
		RLPReader.RLPItem memory rlpItem = RLPReader.toRlpItem(data);
		address result  = RLPReader.toAddress(rlpItem);
		return result;
	}
    
	function get_message_delegatecall(address wasm) internal returns (my_message[] memory){
		uint64 methord = RLPEncode.hash("get_message");
		bytes memory rlpMethord = RLPEncode.encodeUint(uint(methord));
		
		bytes[] memory allInput = new bytes[](1);
		allInput[0] = rlpMethord;
		bytes memory payload = RLPEncode.encodeList(allInput);
		(bool success, bytes memory data) = wasm.delegatecall(payload);
		if(!success){
			revert();
		}
		
		my_message[] memory result  = decode_81f89fa1(data);
		return result;
	}

    function set_my_array_delegatecall(address wasm, my_array memory one) internal {
        uint64 methord = RLPEncode.hash("set_my_array");
        bytes memory rlpMethord = RLPEncode.encodeUint(uint(methord));
        
		bytes memory rlpPara0 = encode_7c89c783(one);
		bytes[] memory allInput = new bytes[](2);
		allInput[0] = rlpMethord;
		allInput[1] = rlpPara0;
		bytes memory payload = RLPEncode.encodeList(allInput);
        (bool success,) = wasm.delegatecall(payload);
        if(!success){
            revert();
        }
    }
    
	function get_my_array_delegatecall(address wasm) internal returns (my_array memory){
		uint64 methord = RLPEncode.hash("get_my_array");
		bytes memory rlpMethord = RLPEncode.encodeUint(uint(methord));
		
		bytes[] memory allInput = new bytes[](1);
		allInput[0] = rlpMethord;
		bytes memory payload = RLPEncode.encodeList(allInput);
		(bool success, bytes memory data) = wasm.delegatecall(payload);
		if(!success){
			revert();
		}
		
		my_array memory result  = decode_7c89c783(data);
		return result;
	}
    
	function get_uint_delegatecall(address wasm) internal returns (uint){
		uint64 methord = RLPEncode.hash("get_uint");
		bytes memory rlpMethord = RLPEncode.encodeUint(uint(methord));
		
		bytes[] memory allInput = new bytes[](1);
		allInput[0] = rlpMethord;
		bytes memory payload = RLPEncode.encodeList(allInput);
		(bool success, bytes memory data) = wasm.delegatecall(payload);
		if(!success){
			revert();
		}
		
		RLPReader.RLPItem memory rlpItem = RLPReader.toRlpItem(data);
		uint result  = RLPReader.toUint(rlpItem);
		return result;
	}
    
	function get_int_delegatecall(address wasm) internal returns (int){
		uint64 methord = RLPEncode.hash("get_int");
		bytes memory rlpMethord = RLPEncode.encodeUint(uint(methord));
		
		bytes[] memory allInput = new bytes[](1);
		allInput[0] = rlpMethord;
		bytes memory payload = RLPEncode.encodeList(allInput);
		(bool success, bytes memory data) = wasm.delegatecall(payload);
		if(!success){
			revert();
		}
		
		RLPReader.RLPItem memory rlpItem = RLPReader.toRlpItem(data);
		int result  = RLPReader.toInt(rlpItem);
		return result;
	}
    
	function get_string_delegatecall(address wasm) internal returns (string memory){
		uint64 methord = RLPEncode.hash("get_string");
		bytes memory rlpMethord = RLPEncode.encodeUint(uint(methord));
		
		bytes[] memory allInput = new bytes[](1);
		allInput[0] = rlpMethord;
		bytes memory payload = RLPEncode.encodeList(allInput);
		(bool success, bytes memory data) = wasm.delegatecall(payload);
		if(!success){
			revert();
		}
		
		RLPReader.RLPItem memory rlpItem = RLPReader.toRlpItem(data);
		string memory result  = RLPReader.toString(rlpItem);
		return result;
	}

    function add_message_delegatecall(address wasm, my_message memory one_message) internal {
        uint64 methord = RLPEncode.hash("add_message");
        bytes memory rlpMethord = RLPEncode.encodeUint(uint(methord));
        
		bytes memory rlpPara0 = encode_114338ea(one_message);
		bytes[] memory allInput = new bytes[](2);
		allInput[0] = rlpMethord;
		allInput[1] = rlpPara0;
		bytes memory payload = RLPEncode.encodeList(allInput);
        (bool success,) = wasm.delegatecall(payload);
        if(!success){
            revert();
        }
    }

    function set_string_delegatecall(address wasm, string memory input) internal {
        uint64 methord = RLPEncode.hash("set_string");
        bytes memory rlpMethord = RLPEncode.encodeUint(uint(methord));
        
		bytes memory rlpPara0 = RLPEncode.encodeString(input);
		bytes[] memory allInput = new bytes[](2);
		allInput[0] = rlpMethord;
		allInput[1] = rlpPara0;
		bytes memory payload = RLPEncode.encodeList(allInput);
        (bool success,) = wasm.delegatecall(payload);
        if(!success){
            revert();
        }
    }

    function set_info_delegatecall(address wasm, info memory input) internal {
        uint64 methord = RLPEncode.hash("set_info");
        bytes memory rlpMethord = RLPEncode.encodeUint(uint(methord));
        
		bytes memory rlpPara0 = encode_06271baf(input);
		bytes[] memory allInput = new bytes[](2);
		allInput[0] = rlpMethord;
		allInput[1] = rlpPara0;
		bytes memory payload = RLPEncode.encodeList(allInput);
        (bool success,) = wasm.delegatecall(payload);
        if(!success){
            revert();
        }
    }
}


contract wasmCaller{

	function callSetUint(address other, uint input) public returns (uint) {
        WASM.set_uint_call(other, input);
        return WASM.get_uint_call(other);
    }

    function callGetUint(address other) public returns (uint) {
        return WASM.get_uint_call(other);
    }

    function callSetInt(address other, int input) public returns (int) {
        WASM.set_int_call(other, input);
        return WASM.get_int_call(other);
    }

    function callGetInt(address other) public returns (int) {
        return WASM.get_int_call(other);
    }

    function callSetString(address other, string memory input) public returns (string memory) {
        WASM.set_string_call(other, input);
        return WASM.get_string_call(other);
    }

    function callGetString(address other) public returns (string memory) {
        return WASM.get_string_call(other);
    }

	function callSetAddress(address other, address input) public returns (address) {
        WASM.set_address_call(other, input);
        return WASM.get_address_call(other);
    }

    function callGetAddress(address other) public returns (address) {
        return WASM.get_address_call(other);
    }

    function callSetInfo(address other, WASM.info memory input) public returns (WASM.info memory) {
        WASM.set_info_call(other, input);
        return WASM.get_info_call(other);
    }

    function callGetInfo(address other) public returns (WASM.info memory) {
        return WASM.get_info_call(other);
    }

	function callAddMessage(address other, WASM.my_message memory input) public returns (WASM.my_message[] memory) {
        WASM.add_message_call(other, input);
        return WASM.get_message_call(other);
    }

    function callGetMessage(address other) public returns (WASM.my_message[] memory) {
        return WASM.get_message_call(other);
    }

    function callSetMyVector(address other, WASM.my_vector memory input) public returns (WASM.my_vector memory) {
        WASM.set_my_vector_call(other, input);
        return WASM.get_my_vector_call(other);
    }

    function callGetMyVector(address other) public returns (WASM.my_vector memory) {
        return WASM.get_my_vector_call(other);
    }

	function callSetMyArray(address other, WASM.my_array memory input) public returns (WASM.my_array memory) {
        WASM.set_my_array_call(other, input);
        return WASM.get_my_array_call(other);
    }

    function callGetMyArray(address other) public returns (WASM.my_array memory) {
        return WASM.get_my_array_call(other);
    }

	function callSetMyMap(address other, WASM.my_map memory input) public returns (WASM.my_map memory) {
        WASM.set_my_map_call(other, input);
        return WASM.get_my_map_call(other);
    }

    function callGetMyMap(address other) public returns (WASM.my_map memory) {
        return WASM.get_my_map_call(other);
    }

}