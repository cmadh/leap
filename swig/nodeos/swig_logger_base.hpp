#include <string>

struct swig_data_wrapper{
    uint8_t *data;
    int32_t size;

    swig_data_wrapper(const char *data, size_t size) {
        this->data = (uint8_t*)data;
        this->size = size;
    }
};

class swig_logger_base {
public:
    virtual ~swig_logger_base() {}

    virtual void on_deep_mind_version(std::string name, uint32_t major, uint32_t minor) {

    }

    virtual void on_abidump_start(uint32_t block_num, uint64_t global_sequence_num) {

    }

    virtual void on_abidump_abi(uint64_t name, swig_data_wrapper abi_data) {

    }

    virtual void on_abidump_end() {

    }

    virtual void on_start_block(uint32_t start_block) {

    }

    virtual void on_accepted_block(uint32_t num, swig_data_wrapper blk) {

    }

    virtual void on_switch_forks(swig_data_wrapper from_id, swig_data_wrapper to_id) {

    }

    virtual void on_error(swig_data_wrapper id, swig_data_wrapper trx) {

    }

    virtual void on_onblock(swig_data_wrapper id, swig_data_wrapper trx) {

    }

    virtual void on_applied_transaction(uint32_t block_num, swig_data_wrapper traces) {

    }

    virtual void on_add_ram_correction(uint32_t action_id, int64_t correction_id, std::string event_id, uint64_t payer, uint64_t delta) {

    }

    virtual void on_input_action(uint32_t action_id) {

    }

    virtual void on_require_recipient(uint32_t action_id) {

    }

    virtual void on_send_inline(uint32_t action_id) {

    }

    virtual void on_send_context_free_inline(uint32_t action_id) {

    }

    virtual void on_cancel_deferred(uint8_t qual, uint32_t action_id, uint64_t sender, swig_data_wrapper sender_id, uint64_t payer,
                            uint32_t published, uint32_t delay, uint32_t expiration, swig_data_wrapper trx_id,
                                    swig_data_wrapper trx) {

    }

    virtual void on_send_deferred(uint8_t qual, uint32_t action_id, uint64_t sender, swig_data_wrapper sender_id, uint64_t payer,
                          uint32_t published, uint32_t delay, uint32_t expiration, swig_data_wrapper trx_id,
                          swig_data_wrapper trx) {

    }

    virtual void on_create_deferred(uint8_t qual, uint32_t action_id, uint64_t sender, swig_data_wrapper sender_id, uint64_t payer,
                            uint32_t published, uint32_t delay, uint32_t expiration, swig_data_wrapper trx_id,
                            swig_data_wrapper trx) {

    }

    virtual void on_fail_deferred(uint32_t action_id) {

    }

    virtual void on_create_table(uint32_t action_id, uint64_t code, uint64_t scope, uint64_t table, uint64_t payer) {

    }

    virtual void on_remove_table(uint32_t action_id, uint64_t code, uint64_t scope, uint64_t table, uint64_t payer) {

    }

    virtual void on_db_store_i64(uint32_t action_id, uint64_t payer, uint64_t table_code, uint64_t scope, uint64_t table_name,
                         uint64_t primkey, swig_data_wrapper ndata) {

    }

    virtual void on_db_update_i64(uint32_t action_id, uint64_t payer, uint64_t table_code, uint64_t scope, uint64_t table_name,
                          uint64_t primkey, swig_data_wrapper odata, swig_data_wrapper ndata) {

    }

    virtual void on_db_remove_i64(uint32_t action_id, uint64_t payer, uint64_t table_code, uint64_t scope, uint64_t table_name,
                     uint64_t primkey, swig_data_wrapper odata) {

    }

    virtual void on_ram_event(uint32_t action_id, std::string event_id, std::string family,
                      std::string operation, std::string legacy_tag, uint64_t payer,
                      uint64_t new_usage, int64_t delta) {

    }

    virtual void on_create_permission(uint32_t action_id, int64_t permission_id, swig_data_wrapper data) {

    }

    virtual void on_modify_permission(uint32_t action_id, int64_t permission_id, swig_data_wrapper opdata, swig_data_wrapper npdata) {

    }

    virtual void on_remove_permission(uint32_t action_id, int64_t permission_id, swig_data_wrapper data) {

    }
};