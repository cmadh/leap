#include "deep_mind_interop_handler.hpp"

#include <eosio/chain/block_state.hpp>
#include <eosio/chain/generated_transaction_object.hpp>
#include <eosio/chain/contract_table_objects.hpp>
#include <eosio/chain/resource_limits_private.hpp>
#include <eosio/chain/permission_object.hpp>
#include <eosio/chain/global_property_object.hpp>
#include <eosio/chain/account_object.hpp>
#include <eosio/chain/protocol_feature_manager.hpp>
#include <fc/crypto/hex.hpp>

namespace {

    void set_trace_elapsed_to_zero(eosio::chain::action_trace& trace) {
        trace.elapsed = fc::microseconds{};
    }

    void set_trace_elapsed_to_zero(eosio::chain::transaction_trace& trace) {
        trace.elapsed = fc::microseconds{};
        for (auto& act_trace : trace.action_traces) {
            set_trace_elapsed_to_zero(act_trace);
        }
    }

}

// deep-mind-INTEROP-handler calling code in wrapping project by inheriting the deep-mind-interop-logger-base-class
namespace eosio::chain {

    deep_mind_interop_handler::deep_mind_interop_handler() = default;

    void deep_mind_interop_handler::update_config(deep_mind_config config)
    {
        _config = std::move(config);
    }

    void deep_mind_interop_handler::update_logger(const std::string& logger_name)
    {
        fc::logger::update( logger_name, _logger );
    }

    /*static const char* prefix(deep_mind_handler::operation_qualifier q) {
        switch(q)
        {
            case deep_mind_handler::operation_qualifier::none: return "";
            case deep_mind_handler::operation_qualifier::modify: return "MODIFY_";
            case deep_mind_handler::operation_qualifier::push: return "PUSH_";
            default: elog("Unknown operation_qualifier"); return "";
        }
    }*/

    void deep_mind_interop_handler::on_startup(chainbase::database& db, uint32_t head_block_num)
    {
        if(new_schedule)
            ilog("new schedule");
        string name = "leap";
        uint major = 13;
        uint minor = 0;
        _interop_logger->on_deep_mind_version(name, major, minor);
        // FIXME: We should probably feed that from CMake directly somehow ...
        //fc_dlog(_logger, "DEEP_MIND_VERSION leap 13 0");

        _interop_logger->on_abidump_start(head_block_num, db.get<dynamic_global_property_object>().global_action_sequence);
        /*fc_dlog(_logger, "ABIDUMP START ${block_num} ${global_sequence_num}",
                ("block_num", head_block_num)
                        ("global_sequence_num", db.get<dynamic_global_property_object>().global_action_sequence)
        );*/
        const auto& idx = db.get_index<account_index>();
        for (auto& row : idx.indices()) {
            if (row.abi.size() != 0) {
                _interop_logger->on_abidump_abi(row.name.to_uint64_t(), interop_data_wrapper(row.abi.data(), row.abi.size()));
                /*fc_dlog(_logger, "ABIDUMP ABI ${contract} ${abi}",
                        ("contract", row.name)
                                ("abi", row.abi)
                );*/
            }
        }
        _interop_logger->on_abidump_end();
        //fc_dlog(_logger, "ABIDUMP END");
    }

    void deep_mind_interop_handler::on_start_block(uint32_t block_num)
    {
        _interop_logger->on_start_block(block_num);
        //fc_dlog(_logger, "START_BLOCK ${block_num}", ("block_num", block_num));
    }

    void deep_mind_interop_handler::on_accepted_block(const std::shared_ptr<block_state>& bsp)
    {
        auto packed_blk = fc::raw::pack(*bsp);

        _interop_logger->on_accepted_block(bsp->block_num, interop_data_wrapper(packed_blk.data(), packed_blk.size()));
        /*fc_dlog(_logger, "ACCEPTED_BLOCK ${num} ${blk}",
                ("num", bsp->block_num)
                        ("blk", fc::to_hex(packed_blk))
        );*/
    }

    void deep_mind_interop_handler::on_switch_forks(const block_id_type& old_head, const block_id_type& new_head)
    {
         _interop_logger->on_switch_forks(interop_data_wrapper(old_head.data(), 32), interop_data_wrapper(new_head.data(), 32));
        /*fc_dlog(_logger, "SWITCH_FORK ${from_id} ${to_id}",
                ("from_id", old_head)
                        ("to_id", new_head)
        );*/
    }

    void deep_mind_interop_handler::on_onerror(const signed_transaction& etrx)
    {
        auto packed_trx = fc::raw::pack(etrx);

        _interop_logger->on_error(interop_data_wrapper(etrx.id().data(), 32), interop_data_wrapper(packed_trx.data(), packed_trx.size()));
        /*fc_dlog(_logger, "TRX_OP CREATE onerror ${id} ${trx}",
                ("id", etrx.id())
                        ("trx", fc::to_hex(packed_trx))
        );*/
    }

    void deep_mind_interop_handler::on_onblock(const signed_transaction& trx)
    {
        auto packed_trx = fc::raw::pack(trx);

        _interop_logger->on_onblock(interop_data_wrapper(trx.id().data(), 32), interop_data_wrapper(packed_trx.data(), packed_trx.size()));
        /*fc_dlog(_logger, "TRX_OP CREATE onblock ${id} ${trx}",
                ("id", trx.id())
                        ("trx", fc::to_hex(packed_trx))
        );*/
    }

    void deep_mind_interop_handler::on_start_transaction()
    {
        _action_id = 0;
    }

    void deep_mind_interop_handler::on_end_transaction()
    {
        _action_id = 0;
    }

    void deep_mind_interop_handler::on_applied_transaction(uint32_t block_num, const transaction_trace_ptr& trace)
    {
        std::vector<char> packed_trace;

        if (_config.zero_elapsed) {
            transaction_trace trace_copy = *trace;
            set_trace_elapsed_to_zero(trace_copy);
            packed_trace = fc::raw::pack(trace_copy);

        } else {
            packed_trace = fc::raw::pack(*trace);
        }

        _interop_logger->on_applied_transaction(block_num, interop_data_wrapper(packed_trace.data(), packed_trace.size()));

        /*fc_dlog(_logger, "APPLIED_TRANSACTION ${block} ${traces}",
                ("block", block_num)
                        ("traces", fc::to_hex(packed_trace))
        );*/
    }

    void deep_mind_interop_handler::on_add_ram_correction(const account_ram_correction_object& rco, uint64_t delta)
    {
        _interop_logger->on_add_ram_correction(_action_id, rco.id._id, _ram_trace.event_id, rco.name.to_uint64_t(), delta);
        /*fc_dlog(_logger, "RAM_CORRECTION_OP ${action_id} ${correction_id} ${event_id} ${payer} ${delta}",
                ("action_id", _action_id)
                        ("correction_id", rco.id._id)
                        ("event_id", _ram_trace.event_id)
                        ("payer", rco.name)
                        ("delta", delta)
        );*/
        _ram_trace = ram_trace();
    }

    void deep_mind_interop_handler::on_preactivate_feature(const protocol_feature& feature)
    {
        /*fc_dlog(_logger, "FEATURE_OP PRE_ACTIVATE ${action_id} ${feature_digest} ${feature}",
                ("action_id", _action_id)
                        ("feature_digest", feature.feature_digest)
                        ("feature", feature.to_variant())
        );*/
    }

    void deep_mind_interop_handler::on_activate_feature(const protocol_feature& feature)
    {
        /*fc_dlog(_logger, "FEATURE_OP ACTIVATE ${feature_digest} ${feature}",
                ("feature_digest", feature.feature_digest)
                        ("feature", feature.to_variant())
        );*/
    }

    void deep_mind_interop_handler::on_input_action()
    {
            _interop_logger->on_input_action(_action_id);
        /*fc_dlog(_logger, "CREATION_OP ROOT ${action_id}",
                ("action_id", _action_id)
        );*/
    }
    void deep_mind_interop_handler::on_end_action()
    {
        ++_action_id;
    }
    void deep_mind_interop_handler::on_require_recipient()
    {
            _interop_logger->on_require_recipient(_action_id);
        /*fc_dlog(_logger, "CREATION_OP NOTIFY ${action_id}",
                ("action_id", _action_id)
        );*/
    }
    void deep_mind_interop_handler::on_send_inline()
    {
            _interop_logger->on_send_inline(_action_id);
        /*fc_dlog(_logger, "CREATION_OP INLINE ${action_id}",
                ("action_id", _action_id)
        );*/
    }
    void deep_mind_interop_handler::on_send_context_free_inline()
    {
        _interop_logger->on_send_context_free_inline(_action_id);
        /*fc_dlog(_logger, "CREATION_OP CFA_INLINE ${action_id}",
                ("action_id", _action_id)
        );*/
    }
    void deep_mind_interop_handler::on_cancel_deferred(operation_qualifier qual, const generated_transaction_object& gto)
    {
        std::vector<char> sender_id = std::vector<char>(16);
        memcpy(&sender_id[0], &gto.sender_id, 16);
        _interop_logger->on_cancel_deferred(uint8_t(qual), _action_id, gto.sender.to_uint64_t(), interop_data_wrapper(&sender_id[0], 16),
                                            gto.payer.to_uint64_t(), gto.published.sec_since_epoch(),
                                            gto.delay_until.sec_since_epoch(), gto.expiration.sec_since_epoch(),
                                            interop_data_wrapper(gto.trx_id.data(), 32), interop_data_wrapper(gto.packed_trx.data(), gto.packed_trx.size()));

        /*fc_dlog(_logger, "DTRX_OP ${qual}CANCEL ${action_id} ${sender} ${sender_id} ${payer} ${published} ${delay} ${expiration} ${trx_id} ${trx}",
                ("qual", prefix(qual))
                        ("action_id", _action_id)
                        ("sender", gto.sender)
                        ("sender_id", gto.sender_id)
                        ("payer", gto.payer)
                        ("published", gto.published)
                        ("delay", gto.delay_until)
                        ("expiration", gto.expiration)
                        ("trx_id", gto.trx_id)
                        ("trx", fc::to_hex(gto.packed_trx.data(), gto.packed_trx.size()))
        );*/
    }
    void deep_mind_interop_handler::on_send_deferred(operation_qualifier qual, const generated_transaction_object& gto)
    {
        std::vector<char> sender_id = std::vector<char>(16);
        memcpy(&sender_id[0], &gto.sender_id, 16);
        _interop_logger->on_send_deferred(uint8_t(qual), _action_id, gto.sender.to_uint64_t(), interop_data_wrapper(&sender_id[0], 16),
                                          gto.payer.to_uint64_t(), gto.published.sec_since_epoch(),
                                          gto.delay_until.sec_since_epoch(), gto.expiration.sec_since_epoch(),
                                          interop_data_wrapper(gto.trx_id.data(), 32), interop_data_wrapper(gto.packed_trx.data(), gto.packed_trx.size()));

        /*fc_dlog(_logger, "DTRX_OP ${qual}CREATE ${action_id} ${sender} ${sender_id} ${payer} ${published} ${delay} ${expiration} ${trx_id} ${trx}",
                ("qual", prefix(qual))
                        ("action_id", _action_id)
                        ("sender", gto.sender)
                        ("sender_id", gto.sender_id)
                        ("payer", gto.payer)
                        ("published", gto.published)
                        ("delay", gto.delay_until)
                        ("expiration", gto.expiration)
                        ("trx_id", gto.trx_id)
                        ("trx", fc::to_hex(gto.packed_trx.data(), gto.packed_trx.size()))
        );*/
    }
    void deep_mind_interop_handler::on_create_deferred(operation_qualifier qual, const generated_transaction_object& gto, const packed_transaction& packed_trx)
    {
        auto packed_signed_trx = fc::raw::pack(packed_trx.get_signed_transaction());
        std::vector<char> sender_id = std::vector<char>(16);
        memcpy(&sender_id[0], &gto.sender_id, 16);
        _interop_logger->on_create_deferred(uint8_t(qual), _action_id, gto.sender.to_uint64_t(), interop_data_wrapper(&sender_id[0], 16),
                                            gto.payer.to_uint64_t(), gto.published.sec_since_epoch(),
                                            gto.delay_until.sec_since_epoch(), gto.expiration.sec_since_epoch(),
                                            interop_data_wrapper(gto.trx_id.data(), 32), interop_data_wrapper(packed_signed_trx.data(), packed_signed_trx.size()));

        /*fc_dlog(_logger, "DTRX_OP ${qual}CREATE ${action_id} ${sender} ${sender_id} ${payer} ${published} ${delay} ${expiration} ${trx_id} ${trx}",
                ("qual", prefix(qual))
                        ("action_id", _action_id)
                        ("sender", gto.sender)
                        ("sender_id", gto.sender_id)
                        ("payer", gto.payer)
                        ("published", gto.published)
                        ("delay", gto.delay_until)
                        ("expiration", gto.expiration)
                        ("trx_id", gto.trx_id)
                        ("trx", fc::to_hex(packed_signed_trx.data(), packed_signed_trx.size()))
        );*/
    }
    void deep_mind_interop_handler::on_fail_deferred()
    {
        _interop_logger->on_fail_deferred(_action_id);
        /*fc_dlog(_logger, "DTRX_OP FAILED ${action_id}",
                ("action_id", _action_id)
        );*/
    }
    void deep_mind_interop_handler::on_create_table(const table_id_object& tid)
    {
        _interop_logger->on_create_table(_action_id, tid.code.to_uint64_t(), tid.scope.to_uint64_t(), tid.table.to_uint64_t(), tid.payer.to_uint64_t());
        /*fc_dlog(_logger, "TBL_OP INS ${action_id} ${code} ${scope} ${table} ${payer}",
                ("action_id", _action_id)
                        ("code", tid.code)
                        ("scope", tid.scope)
                        ("table", tid.table)
                        ("payer", tid.payer)
        );*/
    }
    void deep_mind_interop_handler::on_remove_table(const table_id_object& tid)
    {
        _interop_logger->on_remove_table(_action_id, tid.code.to_uint64_t(), tid.scope.to_uint64_t(), tid.table.to_uint64_t(), tid.payer.to_uint64_t());
        /*fc_dlog(_logger, "TBL_OP REM ${action_id} ${code} ${scope} ${table} ${payer}",
                ("action_id", _action_id)
                        ("code", tid.code)
                        ("scope", tid.scope)
                        ("table", tid.table)
                        ("payer", tid.payer)
        );*/
    }
    void deep_mind_interop_handler::on_db_store_i64(const table_id_object& tid, const key_value_object& kvo)
    {
        _interop_logger->on_db_store_i64(_action_id, kvo.payer.to_uint64_t(), tid.code.to_uint64_t(), tid.scope.to_uint64_t(), tid.table.to_uint64_t(), kvo.primary_key, interop_data_wrapper(kvo.value.data(), kvo.value.size()));
        /*fc_dlog(_logger, "DB_OP INS ${action_id} ${payer} ${table_code} ${scope} ${table_name} ${primkey} ${ndata}",
                ("action_id", _action_id)
                        ("payer", kvo.payer)
                        ("table_code", tid.code)
                        ("scope", tid.scope)
                        ("table_name", tid.table)
                        ("primkey", name(kvo.primary_key))
                        ("ndata", fc::to_hex(kvo.value.data(), kvo.value.size()))
        );*/
    }
    void deep_mind_interop_handler::on_db_update_i64(const table_id_object& tid, const key_value_object& kvo, account_name payer, const char* buffer, std::size_t buffer_size)
    {
        _interop_logger->on_db_update_i64(_action_id, kvo.payer.to_uint64_t(), tid.code.to_uint64_t(), tid.scope.to_uint64_t(), tid.table.to_uint64_t(), kvo.primary_key, interop_data_wrapper(kvo.value.data(), kvo.value.size()), interop_data_wrapper(buffer, buffer_size));
        /*fc_dlog(_logger, "DB_OP UPD ${action_id} ${opayer}:${npayer} ${table_code} ${scope} ${table_name} ${primkey} ${odata}:${ndata}",
                ("action_id", _action_id)
                        ("opayer", kvo.payer)
                        ("npayer", payer)
                        ("table_code", tid.code)
                        ("scope", tid.scope)
                        ("table_name", tid.table)
                        ("primkey", name(kvo.primary_key))
                        ("odata", fc::to_hex(kvo.value.data(),kvo.value.size()))
                        ("ndata", fc::to_hex(buffer, buffer_size))
        );*/
    }
    void deep_mind_interop_handler::on_db_remove_i64(const table_id_object& tid, const key_value_object& kvo)
    {
        _interop_logger->on_db_remove_i64(_action_id, kvo.payer.to_uint64_t(), tid.code.to_uint64_t(), tid.scope.to_uint64_t(), tid.table.to_uint64_t(), kvo.primary_key, interop_data_wrapper(kvo.value.data(), kvo.value.size()));
        /*fc_dlog(_logger, "DB_OP REM ${action_id} ${payer} ${table_code} ${scope} ${table_name} ${primkey} ${odata}",
                ("action_id", _action_id)
                        ("payer", kvo.payer)
                        ("table_code", tid.code)
                        ("scope", tid.scope)
                        ("table_name", tid.table)
                        ("primkey", name(kvo.primary_key))
                        ("odata", fc::to_hex(kvo.value.data(), kvo.value.size()))
        );*/
    }
    void deep_mind_interop_handler::on_init_resource_limits(const resource_limits::resource_limits_config_object& config, const resource_limits::resource_limits_state_object& state)
    {
        /*fc_dlog(_logger, "RLIMIT_OP CONFIG INS ${data}",
                ("data", config)
        );
        fc_dlog(_logger, "RLIMIT_OP STATE INS ${data}",
                ("data", state)
        );*/
    }
    void deep_mind_interop_handler::on_update_resource_limits_config(const resource_limits::resource_limits_config_object& config)
    {
        /*fc_dlog(_logger, "RLIMIT_OP CONFIG UPD ${data}",
                ("data", config)
        );*/
    }
    void deep_mind_interop_handler::on_update_resource_limits_state(const resource_limits::resource_limits_state_object& state)
    {
        /*fc_dlog(_logger, "RLIMIT_OP STATE UPD ${data}",
                ("data", state)
        );*/
    }
    void deep_mind_interop_handler::on_newaccount_resource_limits(const resource_limits::resource_limits_object& limits, const resource_limits::resource_usage_object& usage)
    {
        /*fc_dlog(_logger, "RLIMIT_OP ACCOUNT_LIMITS INS ${data}",
                ("data", limits)
        );
        fc_dlog(_logger, "RLIMIT_OP ACCOUNT_USAGE INS ${data}",
                ("data", usage)
        );*/
    }
    void deep_mind_interop_handler::on_update_account_usage(const resource_limits::resource_usage_object& usage)
    {
        /*fc_dlog(_logger, "RLIMIT_OP ACCOUNT_USAGE UPD ${data}",
                ("data", usage)
        );*/
    }
    void deep_mind_interop_handler::on_set_account_limits(const resource_limits::resource_limits_object& limits)
    {
        /*fc_dlog(_logger, "RLIMIT_OP ACCOUNT_LIMITS UPD ${data}",
                ("data", limits)
        );*/
    }
    void deep_mind_interop_handler::on_ram_trace(std::string&& event_id, const char* family, const char* operation, const char* legacy_tag)
    {
        _ram_trace = ram_trace(std::move(event_id), family, operation, legacy_tag);
    }
    void deep_mind_interop_handler::on_ram_event(account_name account, uint64_t new_usage, int64_t delta)
    {
        _interop_logger->on_ram_event(_action_id, std::string(_ram_trace.event_id), std::string(_ram_trace.family), std::string(_ram_trace.operation),
                                      std::string(_ram_trace.legacy_tag), account.to_uint64_t(), new_usage, delta);
        /*fc_dlog(_logger, "RAM_OP ${action_id} ${event_id} ${family} ${operation} ${legacy_tag} ${payer} ${new_usage} ${delta}",
                ("action_id", _action_id)
                        ("event_id", _ram_trace.event_id)
                        ("family", _ram_trace.family)
                        ("operation", _ram_trace.operation)
                        ("legacy_tag", _ram_trace.legacy_tag)
                        ("payer", account)
                        ("new_usage", new_usage)
                        ("delta", delta)
        );*/
        _ram_trace = ram_trace();
    }

    void deep_mind_interop_handler::on_create_permission(const permission_object& p)
    {
        auto praw = fc::raw::pack(p);

        _interop_logger->on_create_permission(_action_id, p.id._id, interop_data_wrapper(praw.data(), praw.size()));
        /*fc_dlog(_logger, "PERM_OP INS ${action_id} ${permission_id} ${data}",
                ("action_id", _action_id)
                        ("permission_id", p.id)
                        ("data", p)
        );*/
    }
    void deep_mind_interop_handler::on_modify_permission(const permission_object& old_permission, const permission_object& new_permission)
    {
        auto opraw = fc::raw::pack(old_permission);
        auto npraw = fc::raw::pack(new_permission);

        _interop_logger->on_modify_permission(_action_id, new_permission.id._id, interop_data_wrapper(opraw.data(), opraw.size()), interop_data_wrapper(npraw.data(), npraw.size()));
        /*fc_dlog(_logger, "PERM_OP UPD ${action_id} ${permission_id} ${data}",
                ("action_id", _action_id)
                        ("permission_id", new_permission.id)
                        ("data", fc::mutable_variant_object()
                                ("old", old_permission)
                                ("new", new_permission)
                        )
        );*/
    }
    void deep_mind_interop_handler::on_remove_permission(const permission_object& permission)
    {
        auto praw = fc::raw::pack(permission);

        _interop_logger->on_remove_permission(_action_id, permission.id._id, interop_data_wrapper(praw.data(), praw.size()));
        /*fc_dlog(_logger, "PERM_OP REM ${action_id} ${permission_id} ${data}",
                ("action_id", _action_id)
                        ("permission_id", permission.id)
                        ("data", permission)
        );*/
    }

}
