#ifndef LEAP_DEEP_MIND_INTEROP_HANDLER_H
#define LEAP_DEEP_MIND_INTEROP_HANDLER_H

#include <eosio/chain/deep_mind.hpp>
#include <eosio/chain/types.hpp>

#include "interop_logger_base.hpp"

namespace eosio::chain {
    class deep_mind_interop_handler : public deep_mind_handler_base {
    public:

        deep_mind_interop_handler();

        void update_config(deep_mind_config config) override;

        void update_logger(const std::string &logger_name) override;

        void on_startup(chainbase::database &db, uint32_t head_block_num) override;

        void on_start_block(uint32_t block_num) override;

        void on_accepted_block(const std::shared_ptr<block_state> &bsp) override;

        void on_switch_forks(const block_id_type &old_head, const block_id_type &new_head) override;

        void on_onerror(const signed_transaction &etrx) override;

        void on_onblock(const signed_transaction &trx) override;

        void on_start_transaction() override;

        void on_end_transaction() override;

        void on_applied_transaction(uint32_t block_num, const std::shared_ptr<transaction_trace> &trace) override;

        void on_add_ram_correction(const account_ram_correction_object &rco, uint64_t delta) override;

        void on_preactivate_feature(const protocol_feature &feature) override;

        void on_activate_feature(const protocol_feature &feature) override;

        void on_input_action() override;

        void on_end_action() override;

        void on_require_recipient() override;

        void on_send_inline() override;

        void on_send_context_free_inline() override;

        void on_cancel_deferred(operation_qualifier qual, const generated_transaction_object &gto) override;

        void on_send_deferred(operation_qualifier qual, const generated_transaction_object &gto) override;

        void on_create_deferred(operation_qualifier qual, const generated_transaction_object &gto,
                                const packed_transaction &packed_trx) override;

        void on_fail_deferred() override;

        void on_create_table(const table_id_object &tid) override;

        void on_remove_table(const table_id_object &tid) override;

        void on_db_store_i64(const table_id_object &tid, const key_value_object &kvo) override;

        void on_db_update_i64(const table_id_object &tid, const key_value_object &kvo, account_name payer,
                              const char *buffer, std::size_t buffer_size) override;

        void on_db_remove_i64(const table_id_object &tid, const key_value_object &kvo) override;

        void on_init_resource_limits(const resource_limits::resource_limits_config_object &config,
                                     const resource_limits::resource_limits_state_object &state) override;

        void on_update_resource_limits_config(const resource_limits::resource_limits_config_object &config) override;

        void on_update_resource_limits_state(const resource_limits::resource_limits_state_object &state) override;

        void on_newaccount_resource_limits(const resource_limits::resource_limits_object &limits,
                                           const resource_limits::resource_usage_object &usage) override;

        void on_update_account_usage(const resource_limits::resource_usage_object &usage) override;

        void on_set_account_limits(const resource_limits::resource_limits_object &limits) override;

        // The trace is consumed by the next ram_event or ram_correction
        void on_ram_trace(std::string &&event_id, const char *family, const char *operation, const char *legacy_tag) override;

        void on_ram_event(account_name account, uint64_t new_usage, int64_t delta) override;

        void on_create_permission(const permission_object &p) override;

        void on_modify_permission(const permission_object &old_permission, const permission_object &new_permission) override;

        void on_remove_permission(const permission_object &permission) override;

        interop_logger_base *_interop_logger;
    private:
        uint32_t _action_id = 0;
        ram_trace _ram_trace;
        deep_mind_config _config;
        fc::logger _logger;
    };
}

#endif //LEAP_DEEP_MIND_INTEROP_HANDLER_H
