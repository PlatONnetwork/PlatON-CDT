package core

type DB interface {
	Get(key string) []byte
	Set(key string, value []byte)
	Delete(key string)
}

func NewDB() DB {
	return memdb{}
}
type memdb map[string][]byte

func (db memdb) Get(key string) []byte {
	if v , ok := db[key]; ok {
		return v
	}
	return nil
}
func (db memdb) Set(key string, value []byte) {
	db[key] = value
}
func (db memdb) Delete(key string) {
	delete(db, key)
}