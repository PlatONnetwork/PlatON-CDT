package core

import (
	"github.com/AlayaNetwork/Alaya-Go/params"
	"math/big"
	"reflect"

	"github.com/AlayaNetwork/Alaya-Go/common"
	"github.com/AlayaNetwork/Alaya-Go/core/types"
	"github.com/AlayaNetwork/Alaya-Go/crypto/sha3"
	"github.com/AlayaNetwork/Alaya-Go/rlp"
)

type MockStateDB struct {
	Code     map[common.Address][]byte
	CodeHash map[common.Address][]byte
	Nonce    map[common.Address]uint64
	Suicided map[common.Address]bool

	Balance      map[common.Address]*big.Int
	State        map[common.Address]map[string][]byte
	Thash, Bhash common.Hash
	TxIndex      int
	logSize      uint
	Logs         map[common.Hash][]*types.Log
}

func NewMockStateDB() *MockStateDB {
	db := MockStateDB{}
	db.Code = make(map[common.Address][]byte)
	db.CodeHash = make(map[common.Address][]byte)
	db.Nonce = make(map[common.Address]uint64)
	db.Suicided = make(map[common.Address]bool)
	db.State = make(map[common.Address]map[string][]byte)
	db.Balance = make(map[common.Address]*big.Int)
	db.Logs = make(map[common.Hash][]*types.Log)
	return &db
}

func (s *MockStateDB) GetCurrentActiveVersion() uint32 {
	return params.FORKVERSION_0_16_0
}

func (s *MockStateDB) Prepare(thash, bhash common.Hash, ti int) {
	s.Thash = thash
	s.Bhash = bhash
	s.TxIndex = ti
}

func (s *MockStateDB) IntermediateRoot(deleteEmptyObjects bool) common.Hash {
	return common.ZeroHash
}

func (s *MockStateDB) SubBalance(adr common.Address, amount *big.Int) {
	if balance, ok := s.Balance[adr]; ok {
		balance.Sub(balance, amount)
	}
}

func (s *MockStateDB) AddBalance(adr common.Address, amount *big.Int) {

	if balance, ok := s.Balance[adr]; ok {

		balance.Add(balance, amount)
	} else {
		s.Balance[adr] = new(big.Int).Set(amount)
	}
}

func (s *MockStateDB) GetBalance(adr common.Address) *big.Int {
	if balance, ok := s.Balance[adr]; ok {
		return balance
	} else {
		return big.NewInt(0)
	}
}

func (s *MockStateDB) GetState(adr common.Address, key []byte) []byte {
	return s.State[adr][string(key)]
}

func (s *MockStateDB) SetState(adr common.Address, key, val []byte) {
	if len(val) == 0 {
		delete(s.State[adr], string(key))
	} else {
		if stateVal, ok := s.State[adr]; ok {
			stateVal[string(key)] = val
		} else {
			stateVal := make(map[string][]byte)
			stateVal[string(key)] = val
			s.State[adr] = stateVal
		}
	}
}

func (s *MockStateDB) CreateAccount(addr common.Address) {
	storage, ok := s.State[addr]
	if !ok {
		storage = make(map[string][]byte)
		s.State[addr] = storage
	}
}

func (s *MockStateDB) GetNonce(addr common.Address) uint64 {
	nonce, ok := s.Nonce[addr]
	if !ok {
		return 0
	}
	return nonce
}
func (s *MockStateDB) SetNonce(addr common.Address, nonce uint64) {
	s.Nonce[addr] = nonce
}

func (s *MockStateDB) GetCodeHash(addr common.Address) common.Hash {
	hash, ok := s.CodeHash[addr]
	if !ok {
		return common.ZeroHash
	}
	return common.BytesToHash(hash)
}
func (s *MockStateDB) GetCode(addr common.Address) []byte {
	return s.Code[addr]
}
func (s *MockStateDB) SetCode(addr common.Address, code []byte) {
	s.Code[addr] = code

	var h common.Hash
	hw := sha3.NewKeccak256()
	rlp.Encode(hw, code)
	hw.Sum(h[:0])
	s.CodeHash[addr] = h[:]
}
func (s *MockStateDB) GetCodeSize(addr common.Address) int {
	code, ok := s.Code[addr]
	if !ok {
		return 0
	}
	return len(code)
}

func (s *MockStateDB) AddRefund(uint64) {
	return
}
func (s *MockStateDB) SubRefund(uint64) {
	return
}
func (s *MockStateDB) GetRefund() uint64 {
	return 0
}

func (s *MockStateDB) GetCommittedState(common.Address, []byte) []byte {
	return nil
}

//GetState(common.Address, common.Hash) common.Hash
//SetState(common.Address, common.Hash, common.Hash)

func (s *MockStateDB) Suicide(addr common.Address) bool {
	s.Suicided[addr] = true
	s.Balance[addr] = new(big.Int)
	return true
}
func (s *MockStateDB) HasSuicided(addr common.Address) bool {
	suicided, ok := s.Suicided[addr]
	if !ok {
		return false
	}
	return suicided
}

// Exist reports whether the given account exists in state.
// Notably this should also return true for suicided accounts.
func (s *MockStateDB) Exist(common.Address) bool {
	return true
}

// Empty returns whether the given account is empty. Empty
// is defined according to EIP161 (balance = nonce = code = 0).
func (s *MockStateDB) Empty(common.Address) bool {
	return true
}

func (s *MockStateDB) RevertToSnapshot(snapshot int) {
}
func (s *MockStateDB) Snapshot() int {
	return 0
}

func (s *MockStateDB) AddLog(logInfo *types.Log) {
	logInfo.TxHash = s.Thash
	logInfo.BlockHash = s.Bhash
	logInfo.TxIndex = uint(s.TxIndex)
	logInfo.Index = s.logSize
	s.Logs[s.Thash] = append(s.Logs[s.Thash], logInfo)
	s.logSize++
}

func (s *MockStateDB) GetLogs(hash common.Hash) []*types.Log {
	return s.Logs[hash]
}

func (s *MockStateDB) AddPreimage(common.Hash, []byte) {
	return
}

func (s *MockStateDB) ForEachStorage(addr common.Address, fn func([]byte, []byte) bool) {
	state, ok := s.State[addr]
	if !ok {
		return
	}
	for k, v := range state {
		fn([]byte(k), v)
	}
}

func (s *MockStateDB) TxHash() common.Hash {
	return s.Thash
}
func (s *MockStateDB) TxIdx() uint32 {
	return uint32(s.TxIndex)
}

func generateHeader(num *big.Int, parentHash common.Hash, htime *big.Int, coninbase common.Address) *types.Header {
	h := new(types.Header)
	h.Number = num
	h.ParentHash = parentHash
	h.Coinbase = coninbase
	h.Time = htime
	return h
}

func (s *MockStateDB) MigrateStorage(from, to common.Address) {
	s.State[to] = s.State[from]
}

func (s *MockStateDB) Equal(other *MockStateDB) bool {
	if !reflect.DeepEqual(s.Code, other.Code) {
		return false
	}

	if !reflect.DeepEqual(s.CodeHash, other.CodeHash) {
		return false
	}

	if !reflect.DeepEqual(s.Nonce, other.Nonce) {
		return false
	}

	if !reflect.DeepEqual(s.Suicided, other.Suicided) {
		return false
	}

	if !reflect.DeepEqual(s.Balance, other.Balance) {
		return false
	}

	if !reflect.DeepEqual(s.State, other.State) {
		return false
	}

	if s.Thash != other.Thash {
		return false
	}

	if s.Bhash != other.Bhash {
		return false
	}

	if s.TxIndex != other.TxIndex {
		return false
	}

	if s.logSize != other.logSize {
		return false
	}

	if !reflect.DeepEqual(s.Logs, other.Logs) {
		return false
	}

	return true
}

func (lhs *MockStateDB) DeepCopy(rhs *MockStateDB) {
	if nil != rhs.Code {
		lhs.Code = make(map[common.Address][]byte)
		for oneAddress, oneCode := range rhs.Code {
			temp := make([]byte, len(oneCode))
			copy(temp, oneCode)
			lhs.Code[oneAddress] = temp
		}
	}

	if nil != rhs.CodeHash {
		lhs.CodeHash = make(map[common.Address][]byte)
		for oneAddress, oneCodeHash := range rhs.CodeHash {
			temp := make([]byte, len(oneCodeHash))
			copy(temp, oneCodeHash)
			lhs.CodeHash[oneAddress] = temp
		}
	}

	if nil != rhs.Nonce {
		lhs.Nonce = make(map[common.Address]uint64)
		for oneAddress, oneNonce := range rhs.Nonce {
			lhs.Nonce[oneAddress] = oneNonce
		}
	}

	if nil != rhs.Suicided {
		lhs.Suicided = make(map[common.Address]bool)
		for oneAddress, oneSuicided := range rhs.Suicided {
			lhs.Suicided[oneAddress] = oneSuicided
		}
	}

	if nil != rhs.Balance {
		lhs.Balance = make(map[common.Address]*big.Int)
		for oneAddress, oneBalance := range rhs.Balance {
			lhs.Balance[oneAddress] = new(big.Int).Set(oneBalance)
		}
	}

	if nil != rhs.State {
		lhs.State = make(map[common.Address]map[string][]byte)
		for oneAddress, oneState := range rhs.State {
			tempKeyValue := make(map[string][]byte)
			for oneKey, oneValue := range oneState {
				temp := make([]byte, len(oneValue))
				copy(temp, oneValue)
				tempKeyValue[oneKey] = temp
			}
			lhs.State[oneAddress] = tempKeyValue
		}
	}

	lhs.Thash = rhs.Thash

	lhs.Bhash = rhs.Bhash

	lhs.TxIndex = rhs.TxIndex

	lhs.logSize = rhs.logSize

	if nil != rhs.Logs {
		lhs.Logs = make(map[common.Hash][]*types.Log)
		for oneAddress, oneLogs := range rhs.Logs {
			tempLogs := make([]*types.Log, len(oneLogs))
			for i, v := range oneLogs {
				rlpBytes, err := rlp.EncodeToBytes(*v)
				if nil != err {
					panic(err)
				}

				var temp types.Log
				err = rlp.DecodeBytes(rlpBytes, &temp)
				if nil != err {
					panic(err)
				}
				tempLogs[i] = &temp
			}

			lhs.Logs[oneAddress] = tempLogs
		}
	}
}
