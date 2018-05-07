#pragma once

#pragma pack(push, 1)
template <uint64_t ACCOUNT, uint64_t NAME>
struct test_action_action {
   static account_name get_account() {
      return account_name(ACCOUNT);
   }

   static action_name get_name() {
      return action_name(NAME);
   }

   eosio::vector<char> data;

   template <typename DataStream>
   friend DataStream& operator << ( DataStream& ds, const test_action_action& a ) {
      for ( auto c : a.data )
         ds << c;
      return ds;
   }
};

void copy_data(char* data, size_t data_len, eosio::vector<char>& data_out) {
   for (unsigned int i=0; i < data_len; i++)
      data_out.push_back(data[i]);
}

#define SUCCESS_CODE 0
#define PARAMETER_ERROR "6000001 PARAMETER_ERROR" //参数错误
#define BLANCE_NOT_ENOUGH "6000002 BLANCE_NOT_ENOUGH" //余额不足
#define ASK_NOT_EXISTS "6000006 ASK_NOT_EXISTS"//问题不存在
#define TIME_NOT_REACHED "6000007 TIME_NOT_REACHED"//时间未到
#define CANNOT_RELEASE_ASK_REPEAT "6000008 CANNOT_RELEASE_ASK_REPEAT"//问题已经释放，不能重复释放一个问题
#define BALANCE_SUPRE_UINT64_LIMIT "6000009 BALANCE_SUPRE_UINT64_LIMIT"//余额已经超过代码极限（理论永远不会发生，但除非恶搞极限测试）
#define ASK_RELEASED_CANNOT_ANSWER "6000010 ASK_RELEASED_CANNOT_ANSWER"//问题已经释放，不能huida
#define ILLEGAL_ANSWER "6000011 ILLEGAL_ANSWER"//非法回答，比如只有2个答案，结果用户选3或则-1
#define OPTIONS_ANSWERS_COUNT_SHOULE_BIGGER_THAN_ONE  "6000012 OPTIONS_ANSWERS_COUNT_SHOULE_BIGGER_THAN_ONE"
#define CANNOT_MODIFY_OTHERS_ASK  "6000013 CANNOT_MODIFY_OTHERS_ASK"
#define NOT_ENOUGH_OCT_TO_DO_IT  "6000014 NOT_ENOUGH_OCT_TO_DO_IT"//没有足够的oct
#define CANNOT_MODIFY_RELEASED_ASK  "6000015 CANNOT_MODIFY_RELEASED_ASK"//不能回答已经释放的问题
#define YOU_ANSWERED_THIS_QUESTION_EVER  "6000016 YOU_ANSWERED_THIS_QUESTION_EVER"//你已经曾经回答过这个问题
#define NOT_ENOUGH_ALLOWED_OCT_TO_DO_IT  "6000017 NOT_ENOUGH_ALLOWED_OCT_TO_DO_IT"//没有抵押足够的OCT
#define INVALID_QUANTITY "6000018 INVALID_QUANTITY"//无效额数
#define MUST_ISSUE_POSITIVE_QUANTITY "6000019 MUST_ISSUE_POSITIVE_QUANTITY"//额度必须为正整数
#define NSUFFICIENT_AUTHORITY "6000020 NSUFFICIENT_AUTHORITY"//没有足够授权
#define ACCOUNT_IS_FROZEN_BY_ISSUER "6000021 ACCOUNT_IS_FROZEN_BY_ISSUER"//账户倍发行者冻结
#define ALL_TRANSFERS_ARE_FROZEN_BY_ISSUER "6000022 ALL_TRANSFERS_ARE_FROZEN_BY_ISSUER"//不能给被冻结的账户转账
#define ACCOUNT_IS_NOT_WHITE_LISTED "6000023 ACCOUNT_IS_NOT_WHITE_LISTED"//账户没有在白名单
#define CAN_ONLY_TRANSFER_TO_WHITE_LISTED_ACCOUNTS "6000024 CAN_ONLY_TRANSFER_TO_WHITE_LISTED_ACCOUNTS"//只能转账给白名单账户
#define RECEIVER_REQUIRES_WHITELIST_BY_ISSUER "6000025 RECEIVER_REQUIRES_WHITELIST_BY_ISSUER"//货币状态或者接收方状态要求白名单属性
#define INVALID_SYMBOL_NAME "6000026 INVALID_SYMBOL_NAME"//无效的货币
#define TOKEN_WITH_SYMBOL_ALREADY_EXISTS "6000027 TOKEN_WITH_SYMBOL_ALREADY_EXISTS"//货币已经存在
