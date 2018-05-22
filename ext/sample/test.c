int zend_hash_init(HashTable *ht, uint nSize,
    hash_func_t pHashFunction,
    dtor_func_t pDestructor, zend_bool persistent)


int zend_hash_add(HashTable *ht, char *arKey, uint nKeyLen,
    void **pData, uint nDataSize, void *pDest);
