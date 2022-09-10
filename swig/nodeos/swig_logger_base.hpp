#include <string>

class swig_logger_base {
public:
    virtual ~swig_logger_base() {}

    virtual void on_deep_mind_version(const std::string name, const uint major, const uint minor) {

    }

    virtual void on_abidump_start(uint32_t block_num, uint64_t global_sequence_num) {

    }

    virtual void on_abidump_abi(uint64_t name, uint8_t abi_data[]) {

    }

    virtual void on_abidump_end() {

    }

    virtual void on_start_block(uint32_t start_block) {

    }

    virtual void on_accepted_block(uint32_t num, uint8_t blk[]) {

    }

    virtual void on_switch_forks(uint8_t from_id[32], uint8_t to_id[32]) {

    }

    virtual void on_error(uint8_t id[32], uint8_t trx[]) {

    }

    virtual void on_onblock(uint8_t id[32], uint8_t trx[]) {

    }

    virtual void on_applied_transaction(uint32_t i, uint8_t traces[]) {

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

    virtual void on_cancel_deferred(__uint8_t qual, uint32_t action_id, uint64_t sender, uint8_t sender_id[], uint64_t payer,
                            uint32_t published, uint32_t delay, uint32_t expiration, uint8_t trx_id[],
                            uint8_t trx[]) {

    }

    virtual void on_send_deferred(__uint8_t qual, uint32_t action_id, uint64_t sender, uint8_t sender_id[], uint64_t payer,
                          uint32_t published, uint32_t delay, uint32_t expiration, uint8_t trx_id[],
                          uint8_t trx[]) {

    }

    virtual void on_create_deferred(__uint8_t qual, uint32_t action_id, uint64_t sender, uint8_t sender_id[], uint64_t payer,
                            uint32_t published, uint32_t delay, uint32_t expiration, uint8_t trx_id[],
                            uint8_t trx[]) {

    }

    virtual void on_fail_deferred(uint32_t action_id) {

    }

    virtual void on_create_table(uint32_t action_id, uint64_t code, uint64_t scope, uint64_t table, uint64_t payer) {

    }

    virtual void on_remove_table(uint32_t action_id, uint64_t code, uint64_t scope, uint64_t table, uint64_t payer) {

    }

    virtual void on_db_store_i64(uint32_t action_id, uint64_t payer, uint64_t table_code, uint64_t scope, uint64_t table_name,
                         uint64_t primkey, uint8_t ndata[]) {

    }

    virtual void on_db_update_i64(uint32_t action_id, uint64_t payer, uint64_t table_code, uint64_t scope, uint64_t table_name,
                          uint64_t primkey, uint8_t odata[], uint8_t ndata[]) {

    }

    virtual void on_db_remove_i64(uint32_t action_id, uint64_t payer, uint64_t table_code, uint64_t scope, uint64_t table_name,
                     uint64_t primkey, uint8_t odata[]) {

    }

    virtual void on_ram_event(uint32_t action_id, std::basic_string<char> event_id, std::basic_string<char> family,
                      std::basic_string<char> operation, std::basic_string<char> legacy_tag, uint64_t payer,
                      uint64_t new_usage, int64_t delta) {

    }

    virtual void on_create_permission(uint32_t action_id, int64_t permission_id, uint8_t data[]) {

    }

    virtual void on_modify_permission(uint32_t action_id, int64_t permission_id, uint8_t opdata[], uint8_t npdata[]) {

    }

    virtual void on_remove_permission(uint32_t action_id, int64_t permission_id, uint8_t data[]) {

    }
};