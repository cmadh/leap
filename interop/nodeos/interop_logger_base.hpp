#ifndef LEAP_INTEROP_LOGGER_BASE_H
#define LEAP_INTEROP_LOGGER_BASE_H
#include <string>

struct interop_data_wrapper{
    uint8_t *data;
    int32_t size;

    interop_data_wrapper(const char *data, size_t size) {
        this->data = (uint8_t*)data;
        this->size = size;
    }
};

class interop_logger_base {
public:
    virtual ~interop_logger_base() {}

    virtual void on_deep_mind_version(std::string name, uint32_t major, uint32_t minor) {
        //ilog("on_deep_mind_version");
    }

    virtual void on_abidump_start(uint32_t block_num, uint64_t global_sequence_num) {
        //ilog("on_abidump_start");
    }

    virtual void on_abidump_abi(uint64_t name, interop_data_wrapper abi_data) {
        //ilog("on_abidump_abi");
    }

    virtual void on_abidump_end() {
        //ilog("on_abidump_end");
    }

    virtual void on_start_block(uint32_t start_block) {
        //ilog("on_start_block");
    }

    virtual void on_accepted_block(uint32_t num, interop_data_wrapper blk) {
        //ilog("on_accepted_block");
    }

    virtual void on_switch_forks(interop_data_wrapper from_id, interop_data_wrapper to_id) {
        //ilog("on_switch_forks");
    }

    virtual void on_error(interop_data_wrapper id, interop_data_wrapper trx) {
        //ilog("on_error");
    }

    virtual void on_onblock(interop_data_wrapper id, interop_data_wrapper trx) {
        //ilog("on_onblock");
    }

    virtual void on_applied_transaction(uint32_t block_num, interop_data_wrapper traces) {
        //ilog("on_applied_transaction");
    }

    virtual void on_add_ram_correction(uint32_t action_id, int64_t correction_id, std::string event_id, uint64_t payer, uint64_t delta) {
        //ilog("on_add_ram_correction");
    }

    virtual void on_input_action(uint32_t action_id) {
        //ilog("on_input_action");
    }

    virtual void on_require_recipient(uint32_t action_id) {
        //ilog("on_require_recipient");
    }

    virtual void on_send_inline(uint32_t action_id) {
        //ilog("on_send_inline");
    }

    virtual void on_send_context_free_inline(uint32_t action_id) {
        //ilog("on_send_context_free_inline");
    }

    virtual void on_cancel_deferred(uint8_t qual, uint32_t action_id, uint64_t sender, interop_data_wrapper sender_id, uint64_t payer,
                                    uint32_t published, uint32_t delay, uint32_t expiration, interop_data_wrapper trx_id,
                                    interop_data_wrapper trx) {
        //ilog("on_cancel_deferred");
    }

    virtual void on_send_deferred(uint8_t qual, uint32_t action_id, uint64_t sender, interop_data_wrapper sender_id, uint64_t payer,
                                  uint32_t published, uint32_t delay, uint32_t expiration, interop_data_wrapper trx_id,
                                  interop_data_wrapper trx) {
        //ilog("on_send_deferred");
    }

    virtual void on_create_deferred(uint8_t qual, uint32_t action_id, uint64_t sender, interop_data_wrapper sender_id, uint64_t payer,
                                    uint32_t published, uint32_t delay, uint32_t expiration, interop_data_wrapper trx_id,
                                    interop_data_wrapper trx) {
        //ilog("on_create_deferred");
    }

    virtual void on_fail_deferred(uint32_t action_id) {
        //ilog("on_fail_deferred");
    }

    virtual void on_create_table(uint32_t action_id, uint64_t code, uint64_t scope, uint64_t table, uint64_t payer) {
        //ilog("on_create_table");
    }

    virtual void on_remove_table(uint32_t action_id, uint64_t code, uint64_t scope, uint64_t table, uint64_t payer) {
        //ilog("on_remove_table");
    }

    virtual void on_db_store_i64(uint32_t action_id, uint64_t payer, uint64_t table_code, uint64_t scope, uint64_t table_name,
                                 uint64_t primkey, interop_data_wrapper ndata) {
        //ilog("on_db_store_i64");
    }

    virtual void on_db_update_i64(uint32_t action_id, uint64_t payer, uint64_t table_code, uint64_t scope, uint64_t table_name,
                                  uint64_t primkey, interop_data_wrapper odata, interop_data_wrapper ndata) {
        //ilog("on_db_update_i64");
    }

    virtual void on_db_remove_i64(uint32_t action_id, uint64_t payer, uint64_t table_code, uint64_t scope, uint64_t table_name,
                                  uint64_t primkey, interop_data_wrapper odata) {
        //ilog("on_db_remove_i64");
    }

    virtual void on_ram_event(uint32_t action_id, std::string event_id, std::string family,
                      std::string operation, std::string legacy_tag, uint64_t payer,
                      uint64_t new_usage, int64_t delta) {
        //ilog("on_ram_event");
    }

    virtual void on_create_permission(uint32_t action_id, int64_t permission_id, interop_data_wrapper data) {
        //ilog("on_create_permission");
    }

    virtual void on_modify_permission(uint32_t action_id, int64_t permission_id, interop_data_wrapper opdata, interop_data_wrapper npdata) {
        //ilog("on_modify_permission");
    }

    virtual void on_remove_permission(uint32_t action_id, int64_t permission_id, interop_data_wrapper data) {
        //ilog("on_remove_permission");
    }
};

#endif //LEAP_INTEROP_LOGGER_BASE_H