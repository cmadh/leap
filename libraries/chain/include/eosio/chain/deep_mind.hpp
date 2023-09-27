#pragma once

#include <fc/log/logger.hpp>

#include <eosio/chain/types.hpp>

namespace eosio::chain {

class account_ram_correction_object;
class generated_transaction_object;
class table_id_object;
struct key_value_object;
class permission_object;
struct block_state;
struct protocol_feature;
struct signed_transaction;
struct packed_transaction;
struct transaction_trace;
struct ram_trace;
namespace resource_limits {
   class resource_limits_config_object;
   class resource_limits_state_object;
   struct resource_limits_object;
   struct resource_usage_object;
}

#define RAM_EVENT_ID( FORMAT, ... ) \
   fc::format_string( FORMAT, fc::mutable_variant_object()__VA_ARGS__ )

struct ram_trace {
public:
   ram_trace(std::string&& event_id, const char* family, const char* operation, const char* legacy_tag)
      :event_id(std::move(event_id)),family(family),operation(operation),legacy_tag(legacy_tag)
   {}

   std::string  event_id   = "generic";
   const char*  family     = "generic";
   const char*  operation  = "generic";
   const char*  legacy_tag = "generic";

   ram_trace()
   {}
};


class deep_mind_handler_base
{
public:
    virtual ~deep_mind_handler_base() = default;
    struct deep_mind_config {
        bool zero_elapsed = false; // if true, the elapsed field of transaction and action traces is always set to 0 (for reproducibility)
    };

    virtual void update_config(deep_mind_config config) = 0;

    virtual void update_logger(const std::string& logger_name) = 0;
    enum class operation_qualifier { none, modify, push };

    virtual void on_startup(chainbase::database& db, uint32_t head_block_num) = 0;
    virtual void on_start_block(uint32_t block_num) = 0;
    virtual void on_accepted_block(const std::shared_ptr<block_state>& bsp) = 0;
    virtual void on_switch_forks(const block_id_type& old_head, const block_id_type& new_head) = 0;
    virtual void on_onerror(const signed_transaction& etrx) = 0;
    virtual void on_onblock(const signed_transaction& trx) = 0;
    virtual void on_start_transaction() = 0;
    virtual void on_end_transaction() = 0;
    virtual void on_applied_transaction(uint32_t block_num, const std::shared_ptr<transaction_trace>& trace) = 0;
    virtual void on_add_ram_correction(const account_ram_correction_object& rco, uint64_t delta) = 0;
    virtual void on_preactivate_feature(const protocol_feature& feature) = 0;
    virtual void on_activate_feature(const protocol_feature& feature) = 0;
    virtual void on_input_action() = 0;
    virtual void on_end_action() = 0;
    virtual void on_require_recipient() = 0;
    virtual void on_send_inline() = 0;
    virtual void on_send_context_free_inline() = 0;
    virtual void on_cancel_deferred(operation_qualifier qual, const generated_transaction_object& gto) = 0;
    virtual void on_send_deferred(operation_qualifier qual, const generated_transaction_object& gto) = 0;
    virtual void on_create_deferred(operation_qualifier qual, const generated_transaction_object& gto, const packed_transaction& packed_trx) = 0;
    virtual void on_fail_deferred() = 0;
    virtual void on_create_table(const table_id_object& tid) = 0;
    virtual void on_remove_table(const table_id_object& tid) = 0;
    virtual void on_db_store_i64(const table_id_object& tid, const key_value_object& kvo) = 0;
    virtual void on_db_update_i64(const table_id_object& tid, const key_value_object& kvo, account_name payer, const char* buffer, std::size_t buffer_size) = 0;
    virtual void on_db_remove_i64(const table_id_object& tid, const key_value_object& kvo) = 0;
    virtual void on_init_resource_limits(const resource_limits::resource_limits_config_object& config, const resource_limits::resource_limits_state_object& state) = 0;
    virtual void on_update_resource_limits_config(const resource_limits::resource_limits_config_object& config) = 0;
    virtual void on_update_resource_limits_state(const resource_limits::resource_limits_state_object& state) = 0;
    virtual void on_newaccount_resource_limits(const resource_limits::resource_limits_object& limits, const resource_limits::resource_usage_object& usage) = 0;
    virtual void on_update_account_usage(const resource_limits::resource_usage_object& usage) = 0;
    virtual void on_set_account_limits(const resource_limits::resource_limits_object& limits) = 0;
    // The trace is consumed by the next ram_event or ram_correction
    virtual void on_ram_trace(std::string&& event_id, const char* family, const char* operation, const char* legacy_tag) = 0;
    virtual void on_ram_event(account_name account, uint64_t new_usage, int64_t delta) = 0;
    virtual void on_create_permission(const permission_object& p) = 0;
    virtual void on_modify_permission(const permission_object& old_permission, const permission_object& new_permission) = 0;
    virtual void on_remove_permission(const permission_object& permission) = 0;

    bool new_schedule = false;
private:
    uint32_t         _action_id = 0;
    ram_trace        _ram_trace;
    deep_mind_config _config;
    fc::logger       _logger;
};

class deep_mind_handler : public deep_mind_handler_base
{
public:

    void update_config(deep_mind_config config) override;

    void update_logger(const std::string& logger_name) override;

    void on_startup(chainbase::database& db, uint32_t head_block_num) override;
    void on_start_block(uint32_t block_num) override;
    void on_accepted_block(const std::shared_ptr<block_state>& bsp) override;
    void on_switch_forks(const block_id_type& old_head, const block_id_type& new_head) override;
    void on_onerror(const signed_transaction& etrx) override;
    void on_onblock(const signed_transaction& trx) override;
    void on_start_transaction() override;
    void on_end_transaction() override;
    void on_applied_transaction(uint32_t block_num, const std::shared_ptr<transaction_trace>& trace) override;
    void on_add_ram_correction(const account_ram_correction_object& rco, uint64_t delta) override;
    void on_preactivate_feature(const protocol_feature& feature) override;
    void on_activate_feature(const protocol_feature& feature) override;
    void on_input_action() override;
    void on_end_action() override;
    void on_require_recipient() override;
    void on_send_inline() override;
    void on_send_context_free_inline() override;
    void on_cancel_deferred(operation_qualifier qual, const generated_transaction_object& gto) override;
    void on_send_deferred(operation_qualifier qual, const generated_transaction_object& gto) override;
    void on_create_deferred(operation_qualifier qual, const generated_transaction_object& gto, const packed_transaction& packed_trx) override;
    void on_fail_deferred() override;
    void on_create_table(const table_id_object& tid) override;
    void on_remove_table(const table_id_object& tid) override;
    void on_db_store_i64(const table_id_object& tid, const key_value_object& kvo) override;
    void on_db_update_i64(const table_id_object& tid, const key_value_object& kvo, account_name payer, const char* buffer, std::size_t buffer_size) override;
    void on_db_remove_i64(const table_id_object& tid, const key_value_object& kvo) override;
    void on_init_resource_limits(const resource_limits::resource_limits_config_object& config, const resource_limits::resource_limits_state_object& state) override;
    void on_update_resource_limits_config(const resource_limits::resource_limits_config_object& config) override;
    void on_update_resource_limits_state(const resource_limits::resource_limits_state_object& state) override;
    void on_newaccount_resource_limits(const resource_limits::resource_limits_object& limits, const resource_limits::resource_usage_object& usage) override;
    void on_update_account_usage(const resource_limits::resource_usage_object& usage) override;
    void on_set_account_limits(const resource_limits::resource_limits_object& limits) override;
    // The trace is consumed by the next ram_event or ram_correction
    void on_ram_trace(std::string&& event_id, const char* family, const char* operation, const char* legacy_tag) override;
    void on_ram_event(account_name account, uint64_t new_usage, int64_t delta) override;
    void on_create_permission(const permission_object& p) override;
    void on_modify_permission(const permission_object& old_permission, const permission_object& new_permission) override;
    void on_remove_permission(const permission_object& permission) override;
private:
    uint32_t         _action_id = 0;
    ram_trace        _ram_trace;
    deep_mind_config _config;
    fc::logger       _logger;
};

}
