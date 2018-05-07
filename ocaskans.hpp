/**
 *  @file
 *  @copyright defined in eos/LICENSE.txt
 */

#include <eosiolib/eosio.hpp>
#include <eosiolib/db.h>
#include <eosiolib/asset.hpp>
#include<eosiolib/serialize.hpp>
#include "../erc20currency/erc20currency.hpp"
#include"tool.hpp"


const static uint32_t lable_not_release = 0;
const static uint32_t lable_released = 1;


/**
    *  @abi table
*/
struct ask{
    uint64_t       id;
    uint32_t       endtime;
    account_name   from;
    eosio::asset   quantity;
    uint32_t       releasedLable;
    uint32_t       createtime;
    uint64_t       optionanswerscnt;
    std::string    asktitle;
    std::string    optionanswers;

    uint64_t primary_key()const { return id; }

    EOSLIB_SERIALIZE( ask, (id)(endtime)(from)(quantity)(releasedLable)(createtime)(optionanswerscnt)(asktitle)(optionanswers))
};

/**
    *  @abi action ask
*/
struct actask{
    uint64_t       id;
    uint32_t       endtime;
    account_name   from;
    eosio::asset   quantity;
    uint32_t       createtime;
    uint64_t       optionanswerscnt;
    std::string    asktitle;
    std::string    optionanswers;

    uint64_t primary_key()const { return id; }
    EOSLIB_SERIALIZE( actask, (id)(endtime)(from)(quantity)(createtime)(optionanswerscnt)(asktitle)(optionanswers))
};

/*
@abi action answer
*/
struct answer{
    uint64_t  askid;
    account_name  from;
    uint32_t  choosedanswer;
    uint64_t primary_key()const { return askid; }
    uint64_t get_secondary()const { return from; }

    EOSLIB_SERIALIZE(answer, (askid)(from)(choosedanswer))
};

/**
*  @abi table
*/
struct answers{
    uint64_t  askid;
    std::vector<answer> answerlist;
    uint64_t primary_key()const { return askid; }
    EOSLIB_SERIALIZE(answers, (askid)(answerlist))
};


/**
*  @abi action
*/
struct releasemog{
    uint64_t       askid;
    EOSLIB_SERIALIZE(releasemog, (askid))
};


/*
    @abi action
*/
struct rmask{
    uint64_t  askid;
    EOSLIB_SERIALIZE(rmask, (askid))
};

typedef eosio::multi_index<N(ask), ask> askIndex;

typedef eosio::multi_index<N(answers), answers> AnswerIndex;

class ocaskans{

public:
    uint32_t getAnswerCount(uint64_t askid);
    void removeAsk(const rmask & ra);
    void releaseMortgage( const releasemog& rm );
    void store_answer(const answer &a);
    void store_ask(const actask &c);
    void send_deferred_transfer_transaction(oct::transfer as);

    static const uint64_t aksansadmin = N(ocaskans);
    static const uint64_t tokenContract = N(erctoken);
    static const uint64_t currentAdmin = N(ocaskans);
    static const uint64_t answerRequestOCT = 10000;

private:
    void send_deferred_transferfrom_transaction(oct::transferfromact tf);
};







