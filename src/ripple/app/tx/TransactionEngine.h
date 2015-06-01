//------------------------------------------------------------------------------
/*
    This file is part of rippled: https://github.com/ripple/rippled
    Copyright (c) 2012, 2013 Ripple Labs Inc.

    Permission to use, copy, modify, and/or distribute this software for any
    purpose  with  or without fee is hereby granted, provided that the above
    copyright notice and this permission notice appear in all copies.

    THE  SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
    WITH  REGARD  TO  THIS  SOFTWARE  INCLUDING  ALL  IMPLIED  WARRANTIES  OF
    MERCHANTABILITY  AND  FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
    ANY  SPECIAL ,  DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
    WHATSOEVER  RESULTING  FROM  LOSS  OF USE, DATA OR PROFITS, WHETHER IN AN
    ACTION  OF  CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
    OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
*/
//==============================================================================

#ifndef RIPPLE_APP_TX_TRANSACTIONENGINE_H_INCLUDED
#define RIPPLE_APP_TX_TRANSACTIONENGINE_H_INCLUDED

#include <ripple/app/ledger/Ledger.h>
#include <ripple/app/ledger/LedgerEntrySet.h>
#include <utility>

namespace ripple {

// A TransactionEngine applies serialized transactions to a ledger
// It can also, verify signatures, verify fees, and give rejection reasons

struct multisign_t { multisign_t() { } };
static multisign_t const multisign;

// One instance per ledger.
// Only one transaction applied at a time.
class TransactionEngine
    : public CountedObject <TransactionEngine>
{
public:
    static char const* getCountedObjectName () { return "TransactionEngine"; }

private:
    bool enableMultiSign_ =
#if RIPPLE_ENABLE_MULTI_SIGN
        true;
#else
        false;
#endif

    LedgerEntrySet mNodes;

    void txnWrite ();

protected:
    Ledger::pointer mLedger;
    int mTxnSeq = 0;

public:
    TransactionEngine () = default;

    TransactionEngine (Ledger::ref ledger)
        : mLedger (ledger)
    {
        assert (mLedger);
    }

    TransactionEngine (Ledger::ref ledger, multisign_t)
        : enableMultiSign_(true)
        , mLedger (ledger)
    {
        assert (mLedger);
    }

    bool
    enableMultiSign() const
    {
        return enableMultiSign_;
    }

    LedgerEntrySet&
    view ()
    {
        return mNodes;
    }

    Ledger::ref
    getLedger ()
    {
        return mLedger;
    }

    void
    setLedger (Ledger::ref ledger)
    {
        assert (ledger);
        mLedger = ledger;
    }

    std::pair<TER, bool>
    applyTransaction (STTx const&, TransactionEngineParams);

    bool
    checkInvariants (TER result, STTx const& txn, TransactionEngineParams params);
};

inline TransactionEngineParams operator| (const TransactionEngineParams& l1, const TransactionEngineParams& l2)
{
    return static_cast<TransactionEngineParams> (static_cast<int> (l1) | static_cast<int> (l2));
}

inline TransactionEngineParams operator& (const TransactionEngineParams& l1, const TransactionEngineParams& l2)
{
    return static_cast<TransactionEngineParams> (static_cast<int> (l1) & static_cast<int> (l2));
}

} // ripple

#endif
