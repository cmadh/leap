#include "deep_mind_swig_handler.hpp"

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

// deep-mind-SWIG-handler calling code in wrapping project by inheriting the deep-mind-swig-logger-base-class
namespace eosio::chain {

    void deep_mind_swig_handler::update_config(deep_mind_config config)
    {
        _config = std::move(config);
    }

    void deep_mind_swig_handler::update_logger(const std::string& logger_name)
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

    void deep_mind_swig_handler::on_startup(chainbase::database& db, uint32_t head_block_num)
    {
        const string name = "leap";
        const uint major = 13;
        const uint minor = 0;
        _swig_logger.on_deep_mind_version(name, major, minor);
        // FIXME: We should probably feed that from CMake directly somehow ...
        //fc_dlog(_logger, "DEEP_MIND_VERSION leap 13 0");

        _swig_logger.on_abidump_start(head_block_num, db.get<dynamic_global_property_object>().global_action_sequence);
        /*fc_dlog(_logger, "ABIDUMP START ${block_num} ${global_sequence_num}",
                ("block_num", head_block_num)
                        ("global_sequence_num", db.get<dynamic_global_property_object>().global_action_sequence)
        );*/
        const auto& idx = db.get_index<account_index>();
        for (auto& row : idx.indices()) {
            if (row.abi.size() != 0) {
                uint8_t abi_data[row.abi.size()];
                memcpy(abi_data, row.abi.data(), row.abi.size());
                _swig_logger.on_abidump_abi(row.name.to_uint64_t(), abi_data);
                /*fc_dlog(_logger, "ABIDUMP ABI ${contract} ${abi}",
                        ("contract", row.name)
                                ("abi", row.abi)
                );*/
            }
        }
        _swig_logger.on_abidump_end();
        //fc_dlog(_logger, "ABIDUMP END");
    }

    void deep_mind_swig_handler::on_start_block(uint32_t block_num)
    {
        _swig_logger.on_start_block(block_num);
        //fc_dlog(_logger, "START_BLOCK ${block_num}", ("block_num", block_num));
    }

    void deep_mind_swig_handler::on_accepted_block(const std::shared_ptr<block_state>& bsp)
    {
        auto packed_blk = fc::raw::pack(*bsp);
        uint8_t packed_data[packed_blk.size()];
        memcpy(packed_data, packed_blk.data(), packed_blk.size());
        _swig_logger.on_accepted_block(bsp->block_num, packed_data);
        /*fc_dlog(_logger, "ACCEPTED_BLOCK ${num} ${blk}",
                ("num", bsp->block_num)
                        ("blk", fc::to_hex(packed_blk))
        );*/
    }

    void deep_mind_swig_handler::on_switch_forks(const block_id_type& old_head, const block_id_type& new_head)
    {
        uint8_t old_head_data[32];
        memcpy(old_head_data, old_head.data(), 32);

        uint8_t new_head_data[32];
        memcpy(new_head_data, new_head.data(), 32);

        _swig_logger.on_switch_forks(old_head_data, new_head_data);
        /*fc_dlog(_logger, "SWITCH_FORK ${from_id} ${to_id}",
                ("from_id", old_head)
                        ("to_id", new_head)
        );*/
    }

    void deep_mind_swig_handler::on_onerror(const signed_transaction& etrx)
    {
        auto packed_trx = fc::raw::pack(etrx);

        uint8_t packed_trx_data[packed_trx.size()];
        memcpy(packed_trx_data, packed_trx.data(), packed_trx.size());

        uint8_t trx_id[32];
        memcpy(trx_id, etrx.id().data(), 32);

        _swig_logger.on_error(trx_id, packed_trx_data);
        /*fc_dlog(_logger, "TRX_OP CREATE onerror ${id} ${trx}",
                ("id", etrx.id())
                        ("trx", fc::to_hex(packed_trx))
        );*/
    }

    void deep_mind_swig_handler::on_onblock(const signed_transaction& trx)
    {
        auto packed_trx = fc::raw::pack(trx);

        uint8_t packed_trx_data[packed_trx.size()];
        memcpy(packed_trx_data, packed_trx.data(), packed_trx.size());

        uint8_t trx_id[32];
        memcpy(trx_id, trx.id().data(), 32);

        _swig_logger.on_onblock(trx_id, packed_trx_data);
        /*fc_dlog(_logger, "TRX_OP CREATE onblock ${id} ${trx}",
                ("id", trx.id())
                        ("trx", fc::to_hex(packed_trx))
        );*/
    }

    void deep_mind_swig_handler::on_start_transaction()
    {
        _action_id = 0;
    }

    void deep_mind_swig_handler::on_end_transaction()
    {
        _action_id = 0;
    }

    void deep_mind_swig_handler::on_applied_transaction(uint32_t block_num, const transaction_trace_ptr& trace)
    {
        std::vector<char> packed_trace;

        if (_config.zero_elapsed) {
            transaction_trace trace_copy = *trace;
            set_trace_elapsed_to_zero(trace_copy);
            packed_trace = fc::raw::pack(trace_copy);

        } else {
            packed_trace = fc::raw::pack(*trace);
        }

        uint8_t packed_trace_data[packed_trace.size()];
        memcpy(packed_trace_data, packed_trace.data(), packed_trace.size());

        _swig_logger.on_applied_transaction(block_num, packed_trace_data);
        /*fc_dlog(_logger, "APPLIED_TRANSACTION ${block} ${traces}",
                ("block", block_num)
                        ("traces", fc::to_hex(packed_trace))
        );*/
    }

    void deep_mind_swig_handler::on_add_ram_correction(const account_ram_correction_object& rco, uint64_t delta)
    {
        _swig_logger.on_add_ram_correction(_action_id, rco.id._id, _ram_trace.event_id, rco.name.to_uint64_t(), delta);
        /*fc_dlog(_logger, "RAM_CORRECTION_OP ${action_id} ${correction_id} ${event_id} ${payer} ${delta}",
                ("action_id", _action_id)
                        ("correction_id", rco.id._id)
                        ("event_id", _ram_trace.event_id)
                        ("payer", rco.name)
                        ("delta", delta)
        );*/
        _ram_trace = ram_trace();
    }

    void deep_mind_swig_handler::on_preactivate_feature(const protocol_feature& feature)
    {
        /*fc_dlog(_logger, "FEATURE_OP PRE_ACTIVATE ${action_id} ${feature_digest} ${feature}",
                ("action_id", _action_id)
                        ("feature_digest", feature.feature_digest)
                        ("feature", feature.to_variant())
        );*/
    }

    void deep_mind_swig_handler::on_activate_feature(const protocol_feature& feature)
    {
        /*fc_dlog(_logger, "FEATURE_OP ACTIVATE ${feature_digest} ${feature}",
                ("feature_digest", feature.feature_digest)
                        ("feature", feature.to_variant())
        );*/
    }

    void deep_mind_swig_handler::on_input_action()
    {
        _swig_logger.on_input_action(_action_id);
        /*fc_dlog(_logger, "CREATION_OP ROOT ${action_id}",
                ("action_id", _action_id)
        );*/
    }
    void deep_mind_swig_handler::on_end_action()
    {
        ++_action_id;
    }
    void deep_mind_swig_handler::on_require_recipient()
    {
        _swig_logger.on_require_recipient(_action_id);
        /*fc_dlog(_logger, "CREATION_OP NOTIFY ${action_id}",
                ("action_id", _action_id)
        );*/
    }
    void deep_mind_swig_handler::on_send_inline()
    {
        _swig_logger.on_send_inline(_action_id);
        /*fc_dlog(_logger, "CREATION_OP INLINE ${action_id}",
                ("action_id", _action_id)
        );*/
    }
    void deep_mind_swig_handler::on_send_context_free_inline()
    {
        _swig_logger.on_send_context_free_inline(_action_id);
        /*fc_dlog(_logger, "CREATION_OP CFA_INLINE ${action_id}",
                ("action_id", _action_id)
        );*/
    }
    void deep_mind_swig_handler::on_cancel_deferred(operation_qualifier qual, const generated_transaction_object& gto)
    {
        uint8_t packed_trx_data[gto.packed_trx.size()];
        memcpy(packed_trx_data, gto.packed_trx.data(), gto.packed_trx.size());

        uint8_t trx_id_data[32];
        memcpy(trx_id_data, gto.trx_id.data(), 32);

        auto senderidp = reinterpret_cast<uint8_t*>(uint128_t(gto.sender_id));
        uint8_t senderid_data[16];
        memcpy(senderid_data, senderidp, 16);

        _swig_logger.on_cancel_deferred(uint8_t(qual), _action_id, gto.sender.to_uint64_t(), senderid_data,
                                        gto.payer.to_uint64_t(), gto.published.sec_since_epoch(),
                                        gto.delay_until.sec_since_epoch(), gto.expiration.sec_since_epoch(),
                                        trx_id_data, packed_trx_data);

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
    void deep_mind_swig_handler::on_send_deferred(operation_qualifier qual, const generated_transaction_object& gto)
    {
        uint8_t packed_trx_data[gto.packed_trx.size()];
        memcpy(packed_trx_data, gto.packed_trx.data(), gto.packed_trx.size());

        uint8_t trx_id_data[32];
        memcpy(trx_id_data, gto.trx_id.data(), 32);

        auto senderidp = reinterpret_cast<uint8_t*>(uint128_t(gto.sender_id));
        uint8_t senderid_data[16];
        memcpy(senderid_data, senderidp, 16);

        _swig_logger.on_send_deferred(uint8_t(qual), _action_id, gto.sender.to_uint64_t(), senderid_data,
                                      gto.payer.to_uint64_t(), gto.published.sec_since_epoch(),
                                      gto.delay_until.sec_since_epoch(), gto.expiration.sec_since_epoch(),
                                      trx_id_data, packed_trx_data);

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
    void deep_mind_swig_handler::on_create_deferred(operation_qualifier qual, const generated_transaction_object& gto, const packed_transaction& packed_trx)
    {
        auto packed_signed_trx = fc::raw::pack(packed_trx.get_signed_transaction());

        uint8_t packed_trx_data[gto.packed_trx.size()];
        memcpy(packed_trx_data, gto.packed_trx.data(), gto.packed_trx.size());

        uint8_t trx_id_data[32];
        memcpy(trx_id_data, gto.trx_id.data(), 32);

        auto senderidp = reinterpret_cast<uint8_t*>(uint128_t(gto.sender_id));
        uint8_t senderid_data[16];
        memcpy(senderid_data, senderidp, 16);

        _swig_logger.on_create_deferred(uint8_t(qual), _action_id, gto.sender.to_uint64_t(), senderid_data,
                                        gto.payer.to_uint64_t(), gto.published.sec_since_epoch(),
                                        gto.delay_until.sec_since_epoch(), gto.expiration.sec_since_epoch(),
                                        trx_id_data, packed_trx_data);

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
    void deep_mind_swig_handler::on_fail_deferred()
    {
        _swig_logger.on_fail_deferred(_action_id);
        fc_dlog(_logger, "DTRX_OP FAILED ${action_id}",
                ("action_id", _action_id)
        );
    }
    void deep_mind_swig_handler::on_create_table(const table_id_object& tid)
    {
        _swig_logger.on_create_table(_action_id, tid.code.to_uint64_t(), tid.scope.to_uint64_t(), tid.table.to_uint64_t(), tid.payer.to_uint64_t());
        /*fc_dlog(_logger, "TBL_OP INS ${action_id} ${code} ${scope} ${table} ${payer}",
                ("action_id", _action_id)
                        ("code", tid.code)
                        ("scope", tid.scope)
                        ("table", tid.table)
                        ("payer", tid.payer)
        );*/
    }
    void deep_mind_swig_handler::on_remove_table(const table_id_object& tid)
    {
        _swig_logger.on_remove_table(_action_id, tid.code.to_uint64_t(), tid.scope.to_uint64_t(), tid.table.to_uint64_t(), tid.payer.to_uint64_t());
        /*fc_dlog(_logger, "TBL_OP REM ${action_id} ${code} ${scope} ${table} ${payer}",
                ("action_id", _action_id)
                        ("code", tid.code)
                        ("scope", tid.scope)
                        ("table", tid.table)
                        ("payer", tid.payer)
        );*/
    }
    void deep_mind_swig_handler::on_db_store_i64(const table_id_object& tid, const key_value_object& kvo)
    {
        uint8_t ndata[kvo.value.size()];
        memcpy(ndata, kvo.value.data(), kvo.value.size());

        _swig_logger.on_db_store_i64(_action_id, kvo.payer.to_uint64_t(), tid.code.to_uint64_t(), tid.scope.to_uint64_t(), tid.table.to_uint64_t(), kvo.primary_key, ndata);
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
    void deep_mind_swig_handler::on_db_update_i64(const table_id_object& tid, const key_value_object& kvo, account_name payer, const char* buffer, std::size_t buffer_size)
    {
        uint8_t ndata[buffer_size];
        memcpy(ndata, buffer, buffer_size);

        uint8_t odata[kvo.value.size()];
        memcpy(odata, kvo.value.data(), kvo.value.size());

        _swig_logger.on_db_update_i64(_action_id, kvo.payer.to_uint64_t(), tid.code.to_uint64_t(), tid.scope.to_uint64_t(), tid.table.to_uint64_t(), kvo.primary_key, odata, ndata);
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
    void deep_mind_swig_handler::on_db_remove_i64(const table_id_object& tid, const key_value_object& kvo)
    {
        uint8_t odata[kvo.value.size()];
        memcpy(odata, kvo.value.data(), kvo.value.size());

        _swig_logger.on_db_remove_i64(_action_id, kvo.payer.to_uint64_t(), tid.code.to_uint64_t(), tid.scope.to_uint64_t(), tid.table.to_uint64_t(), kvo.primary_key, odata);
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
    void deep_mind_swig_handler::on_init_resource_limits(const resource_limits::resource_limits_config_object& config, const resource_limits::resource_limits_state_object& state)
    {
        /*fc_dlog(_logger, "RLIMIT_OP CONFIG INS ${data}",
                ("data", config)
        );
        fc_dlog(_logger, "RLIMIT_OP STATE INS ${data}",
                ("data", state)
        );*/
    }
    void deep_mind_swig_handler::on_update_resource_limits_config(const resource_limits::resource_limits_config_object& config)
    {
        /*fc_dlog(_logger, "RLIMIT_OP CONFIG UPD ${data}",
                ("data", config)
        );*/
    }
    void deep_mind_swig_handler::on_update_resource_limits_state(const resource_limits::resource_limits_state_object& state)
    {
        /*fc_dlog(_logger, "RLIMIT_OP STATE UPD ${data}",
                ("data", state)
        );*/
    }
    void deep_mind_swig_handler::on_newaccount_resource_limits(const resource_limits::resource_limits_object& limits, const resource_limits::resource_usage_object& usage)
    {
        /*fc_dlog(_logger, "RLIMIT_OP ACCOUNT_LIMITS INS ${data}",
                ("data", limits)
        );
        fc_dlog(_logger, "RLIMIT_OP ACCOUNT_USAGE INS ${data}",
                ("data", usage)
        );*/
    }
    void deep_mind_swig_handler::on_update_account_usage(const resource_limits::resource_usage_object& usage)
    {
        /*fc_dlog(_logger, "RLIMIT_OP ACCOUNT_USAGE UPD ${data}",
                ("data", usage)
        );*/
    }
    void deep_mind_swig_handler::on_set_account_limits(const resource_limits::resource_limits_object& limits)
    {
        /*fc_dlog(_logger, "RLIMIT_OP ACCOUNT_LIMITS UPD ${data}",
                ("data", limits)
        );*/
    }
    void deep_mind_swig_handler::on_ram_trace(std::string&& event_id, const char* family, const char* operation, const char* legacy_tag)
    {
        _ram_trace = ram_trace(std::move(event_id), family, operation, legacy_tag);
    }
    void deep_mind_swig_handler::on_ram_event(account_name account, uint64_t new_usage, int64_t delta)
    {
        _swig_logger.on_ram_event(_action_id, std::string(_ram_trace.event_id), std::string(_ram_trace.family), std::string(_ram_trace.operation),
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

    void deep_mind_swig_handler::on_create_permission(const permission_object& p)
    {
        auto praw = fc::raw::pack(p);
        uint8_t pdata[praw.size()];
        memcpy(pdata, praw.data(), praw.size());

        _swig_logger.on_create_permission(_action_id, p.id._id, pdata);
        /*fc_dlog(_logger, "PERM_OP INS ${action_id} ${permission_id} ${data}",
                ("action_id", _action_id)
                        ("permission_id", p.id)
                        ("data", p)
        );*/
    }
    void deep_mind_swig_handler::on_modify_permission(const permission_object& old_permission, const permission_object& new_permission)
    {
        auto opraw = fc::raw::pack(old_permission);
        uint8_t opdata[opraw.size()];
        memcpy(opdata, opraw.data(), opraw.size());


        auto npraw = fc::raw::pack(new_permission);
        uint8_t npdata[npraw.size()];
        memcpy(npdata, npraw.data(), npraw.size());

        _swig_logger.on_modify_permission(_action_id, new_permission.id._id, opdata, npdata);
        /*fc_dlog(_logger, "PERM_OP UPD ${action_id} ${permission_id} ${data}",
                ("action_id", _action_id)
                        ("permission_id", new_permission.id)
                        ("data", fc::mutable_variant_object()
                                ("old", old_permission)
                                ("new", new_permission)
                        )
        );*/
    }
    void deep_mind_swig_handler::on_remove_permission(const permission_object& permission)
    {
        auto praw = fc::raw::pack(permission);
        uint8_t pdata[praw.size()];
        memcpy(pdata, praw.data(), praw.size());

        _swig_logger.on_remove_permission(_action_id, permission.id._id, pdata);
        /*fc_dlog(_logger, "PERM_OP REM ${action_id} ${permission_id} ${data}",
                ("action_id", _action_id)
                        ("permission_id", permission.id)
                        ("data", permission)
        );*/
    }

}
