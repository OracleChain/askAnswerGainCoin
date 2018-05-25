/**
 *  @file
 *  @copyright defined in eos/LICENSE.txt
 */

#include <eosiolib/db.h>
#include <eosiolib/types.hpp>
#include <eosiolib/transaction.hpp>
#include "../EOSDACToken/eosdactoken.hpp"


#include "tool.hpp"
#include "ocaskans.hpp"


/**
 *  The init() and apply() methods must have C calling convention so that the blockchain can lookup and
 *  call these methods.
 */

void ocaskans::send_deferred_transferfrom_transaction(transferfromact tf){
    eosio::print(" >>>ocaskans transferfromact from:", tf.from);
    eosio::print(">>>ocaskans transferfromact to:", tf.to);
    eosio::print(">>>ocaskans transferfromact quantity:", tf.quantity);
    eosio::print(">>>ocaskans transferfromact:", "<<<");


    eosio::asset fromBalance = eosdactoken(tokenContract).get_balance(tf.from, tf.quantity.symbol.name());
    eosio_assert(fromBalance.amount >= tf.quantity.amount,NOT_ENOUGH_OCT_TO_DO_IT);

    uint64_t allowed = eosdactoken(tokenContract).allowanceOf(tf.from, tf.to, tf.quantity.symbol.name());
    eosio_assert(allowed>= tf.quantity.amount, NOT_ENOUGH_ALLOWED_OCT_TO_DO_IT);


    using namespace eosio;
    auto trx = eosio::transaction();

    test_action_action<tokenContract, N(transferfrom)> transferfromcurrency;
    copy_data((char*)&tf, sizeof(tf), transferfromcurrency.data);
    trx.actions.emplace_back(vector<permission_level>{{currentAdmin, N(active)}},transferfromcurrency);
    trx.delay_sec = 0;

    uint64_t trxId = current_time();
    trxId += tf.from;
    trxId += tf.to;
    trxId += tf.quantity.symbol.name();
    trxId += tf.quantity.amount;

    trx.send( trxId, currentAdmin);
}

void ocaskans::send_deferred_transfer_transaction(std::vector<transfer> vectf) {

    using namespace eosio;
    auto trx = eosio::transaction();

    uint64_t trxId = current_time();
    for(std::vector<transfer>::iterator tf = vectf.begin(); tf!= vectf.end(); tf++){
        eosio::print(">>>ocaskans transfer from:", tf->from);
        eosio::print("ocaskans transfer to:", tf->to);
        eosio::print("ocaskans transfer memo:", tf->memo.c_str());
        eosio::print("ocaskans transfer quantity:", tf->quantity, "<<<");

        eosio::asset fromBalance = eosdactoken(tokenContract).get_balance(tf->from, tf->quantity.symbol.name());
        eosio_assert(fromBalance.amount >= tf->quantity.amount,NOT_ENOUGH_OCT_TO_DO_IT);

        transfer onetf = *tf;
        test_action_action<tokenContract, N(transfer)> transfercurrency;
        copy_data((char*)&onetf, sizeof(onetf), transfercurrency.data);
        trx.actions.emplace_back(vector<permission_level>{{currentAdmin, N(active)}},transfercurrency);

        trxId+=tf->from;
        trxId+=tf->to;
        trxId+=tf->quantity.symbol.name();
        trxId+=tf->quantity.amount;
    }
   trx.delay_sec = 0;

   trx.send( trxId, currentAdmin );
}

/*
you can modify you self asks, or else will add a new ask
*/
void ocaskans::store_ask(const actask &askItemPar){
    actask c = askItemPar;
    require_auth(c.from);
    eosio_assert(c.optionanswerscnt>2 && c.optionanswerscnt<10000, OPTIONS_ANSWERS_COUNT_SHOULE_BIGGER_THAN_ONE);

    askIndex askItem(currentAdmin, aksansadmin);
    auto to = askItem.find( c.id );

    transferfromact transAct;
    transAct.from = askItemPar.from;
    transAct.to = aksansadmin;
    transAct.quantity = askItemPar.quantity;
    send_deferred_transferfrom_transaction(transAct);


    uint64_t idincrease = c.id;
    if(to != askItem.end())
    {
        to = askItem.find(idincrease);
        while(to != askItem.end())
        {
             idincrease++;
             to = askItem.find(idincrease);
        }
    }
    c.id = idincrease;

    askItem.emplace(c.from, [&]( auto& s ) {
        s.id = c.id;
        s.createtime = now();
        s.endtime    = c.createtime+c.endtime;;
        s.from        = c.from;
        s.quantity = c.quantity;
        s.releasedLable = lable_not_release;
        s.optionanswerscnt = c.optionanswerscnt;
        s.asktitle = c.asktitle;
        s.optionanswers = c.optionanswers;
     });

     eosio::print("answersid:", c.id);
}

void ocaskans::store_answer(const answer &a){

    require_auth(a.from);

    askIndex askItem(currentAdmin, aksansadmin);
    auto to = askItem.find( a.askid);
    if(to == askItem.end()){
        eosio_assert(false, ASK_NOT_EXISTS);
    }

    eosio_assert(to->releasedLable==lable_not_release, CANNOT_RELEASE_ASK_REPEAT);
    eosio_assert((to->optionanswerscnt>=a.choosedanswer)&&(a.choosedanswer>0), ILLEGAL_ANSWER);


    transferfromact transAct;
    transAct.from = a.from;
    transAct.to = aksansadmin;
    transAct.quantity = to->quantity;
    transAct.quantity.amount = answerRequestOCT;
    send_deferred_transferfrom_transaction(transAct);


    AnswerIndex ai(currentAdmin, aksansadmin);
    auto ite = ai.find(a.askid);
    if (ite != ai.end()) {
        std::vector<answer> answersList = ite->answerlist;
        auto answerItem = answersList.begin();
        while(answerItem != answersList.end()){
            if(answerItem->from == a.from){
                eosio_assert(false, YOU_ANSWERED_THIS_QUESTION_EVER);
            }
            ++answerItem;
        }
        answersList.push_back(a);
        ai.modify(ite, 0, [&](auto &s){
            s.askid = a.askid;
            s.answerlist = answersList;
        });
    }
    else{
        ai.emplace(aksansadmin, [&](auto &s){
            s.askid = a.askid;
            std::vector<answer> ans;
            ans.push_back(a);
            s.answerlist = ans;
        });
    }
}

void ocaskans::releaseMortgage( const releasemog& rm ) {
    require_auth(aksansadmin);

    askIndex askContainer(currentAdmin, aksansadmin);
    auto askItem = askContainer.find(rm.askid);
    if(askItem == askContainer.end()){
         eosio_assert(false, ASK_NOT_EXISTS);
    }
    else{
          eosio_assert(askItem->releasedLable != lable_released, CANNOT_RELEASE_ASK_REPEAT);
          eosio_assert(askItem->endtime<=now(), TIME_NOT_REACHED);

          uint64_t nTotalCntAnswer = getAnswerCount(rm.askid);
          if(nTotalCntAnswer>0)
          {
              uint64_t avg = askItem->quantity.amount/nTotalCntAnswer;
              if(avg>0){
                  AnswerIndex answerContainer(currentAdmin, aksansadmin);
                  auto answerItem = answerContainer.find(rm.askid);
                  if(answerItem != answerContainer.end()){
                      auto ansItem = answerItem->answerlist.begin();
                      std::vector<transfer> vectrs;
                      while(ansItem != answerItem->answerlist.end()){

                          transfer trs;
                          trs.from = aksansadmin;
                          trs.to = ansItem->from;
                          trs.memo = std::string("");
                          trs.quantity = askItem->quantity;
                          trs.quantity.amount = avg+answerRequestOCT;
                          vectrs.push_back(trs);
                          ++ansItem;
                      }
                      send_deferred_transfer_transaction(vectrs);
                  }
              }
          }else{
               eosio_assert(false, LOGIC_ERROR);
          }

          askContainer.modify(askItem, 0, [&]( auto& s ) {
              s.id = askItem->id;
              s.endtime    = askItem->endtime;
              s.from        = askItem->from;
              s.quantity = askItem->quantity;
              s.releasedLable = lable_released;
              s.createtime = askItem->createtime;
              s.optionanswerscnt = askItem->optionanswerscnt;
              s.asktitle = askItem->asktitle;
              s.optionanswers = askItem->optionanswers;
           });
    }
}

void ocaskans::removeAsk(const rmask & ra){
    askIndex askContainer(currentAdmin, aksansadmin);
    auto  askItem = askContainer.find(ra.askid);
    if(askItem != askContainer.end()){
        askContainer.erase(askItem);
    }
}

uint32_t ocaskans::getAnswerCount(uint64_t askid){
    AnswerIndex ai(currentAdmin, aksansadmin);
    auto answerItem = ai.find(askid);
    uint32_t count = 0;
    if(answerItem == ai.end()){
        return 0;
    }
    auto ansItem = answerItem->answerlist.begin();
    while(ansItem != answerItem->answerlist.end()){
        ++count;
        ++ansItem;
    }
    eosio::print("count:", count);
    return count;
}


extern "C" {

using namespace eosio;

void apply( uint64_t receiver, uint64_t code, uint64_t action ) {

    switch(action)
    {
        case N(ask):
        {
            ocaskans().store_ask( unpack_action_data<actask>() );
            break;
        }
        case N(answer):
        {
            ocaskans().store_answer( unpack_action_data<answer>() );
            break;
        }
        case N(releasemog):
        {
            ocaskans().releaseMortgage( unpack_action_data<releasemog>() );
            break;
        }
        case N(rmask):
        {
            ocaskans().removeAsk( unpack_action_data<rmask>() );
            break;
        }
    }
  }
}

// extern "C"


