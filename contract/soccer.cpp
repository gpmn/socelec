#include <string>
#include <vector>
#include <eosiolib/eosio.hpp>
#include <eosiolib/asset.hpp>
#include <eosiolib/contract.hpp>
#include <eosiolib/types.h>
#include <eosiolib/currency.hpp>

using namespace std;

using eosio::name;
using eosio::asset;
using eosio::permission_level;
using eosio::action;
using eosio::print;
using eosio::contract;

#define N(X) ::eosio::string_to_name(#X)

#define MATCH_STATUS_OPEN       1
#define MATCH_STATUS_FREEZE     0
#define MATCH_STATUS_CLOSE      (-1)
#define MAX_CONCEDE             5
#define MIN_CONCEDE             (-5)
#define MAX_AMOUNT              10000   // 1 SYS AT MOST
#define MIN_AMOUNT              1000 // 0.1 SYS AT LEAST
#define DEFAULT_DISCOUNT_RATIO  10
#define MAX_DISCOUNT_RATIO      30
#define NOBET_INTERVAL          (10 * 60) // begin 10 mins of game, freeze bet

class soccer: public contract {
     struct match;
     struct game;
     
public:
     using contract::contract;
     soccer(account_name self):contract(self),
                               socctrls(self, self),
                               credits(self, self),
                               matches(self, self),
                               games(self, self){
     }
                            
     void transfer(account_name code, const eosio::currency::transfer& t){
          // print("transfer from ", eosio::name{t.from}, " to ", eosio::name{t.to}, "\n");
          // print("transfer code : ", eosio::name{code}, ", val : ", code, "\n");

          auto ctrlItr = socctrls.begin();
          if( ctrlItr == socctrls.end() ) {
               ctrlItr = socctrls.emplace(_self, [&](auto& a){
                         a.id = 0;
                         a.next_index = 0;
                         a.discnt = DEFAULT_DISCOUNT_RATIO;
                    });
          }          
          eosio_assert(!ctrlItr->ext1, "contract freezed"); // ext1 means contractFreezed
          eosio_assert(code == N(eosio.token), "invalid contract source");
          if(t.from == _self){
               //eosio::print("ignore transfer notification from myself\n");
               return;
          }
          
          if(t.to != _self){
               //eosio::print("ignore transfer notification not to myself");
               return;
          }
      
          // print(eosio::name{t.from}," transferring ", t.quantity.symbol, " to ", eosio::name{t.to});
          eosio_assert(t.quantity.symbol == CORE_SYMBOL,"accept EOS only");
          eosio_assert(t.quantity.is_valid(), "transfer invalid quantity");
          eosio_assert(t.quantity.amount > 0, "amount not positive");

          auto accitr = credits.find(t.from);
          if(accitr == credits.end()){ // new credit
               //eosio::print("new credit for ", eosio::name{t.from}, "\n");
               accitr = credits.emplace(_self, [&](auto& c){
                         c.account = name{t.from};
                         c.money = t.quantity;
                         c.freezed = asset(0, CORE_SYMBOL);
                         c.winCnt = 0;
                         c.loseCnt = 0;
                         c.winMoney = asset(0, CORE_SYMBOL);
                         c.loseMoney = asset(0, CORE_SYMBOL);
                    });
          }else{                // update credit
               //eosio::print("update credit for ", eosio::name{t.from}, "\n");
               credits.modify(accitr, 0, [&](auto& c){
                         eosio_assert(c.money + t.quantity > c.money, "quantity overflow");
                         c.money += t.quantity;
                    });
          }
          socctrls.modify(ctrlItr, 0, [&](auto &c){
                    c.stat_incoming += t.quantity;
                    c.stat_holding += t.quantity;
                    c.stat_credit += t.quantity;
               });
     }

     // @abi action
     struct withdraw{
          account_name account;
          asset quantity;
     };
          
     void do_withdraw(account_name account, asset quantity){
          //eosio::print("will withdraw for ", account);
          require_auth(account);
          eosio_assert(quantity.symbol == CORE_SYMBOL,"contract accept EOS only");
          eosio_assert(quantity.is_valid(), "transfer invalid quantity");
          eosio_assert(quantity.amount > 0, "amount must positive");

          auto ctrlItr = socctrls.begin();
          eosio_assert(ctrlItr != socctrls.end() && !ctrlItr->ext1, "contract freezed"); // ext1 means contractFreezed

          auto credIter = credits.find(account);
          eosio_assert(credIter != credits.end(), "no credit yet");
          credits.modify(credIter, 0, [&](auto& a){
                    eosio_assert(a.money >= quantity, "insufficient credit" );
                    a.money -= quantity;
                    a.freezed += quantity;
               });
     }

     // @abi action
     struct cancelwd{
          account_name account;
     };

     void do_cancelwd(account_name account){
          //eosio::print("will withdraw for ", account);
          require_auth(account);
      
          auto ctrlItr = socctrls.begin();
          eosio_assert(ctrlItr != socctrls.end() && !ctrlItr->ext1, "contract freezed"); // ext1 means contractFreezed          
          auto credIter = credits.find(account);
          eosio_assert(credIter != credits.end(), "no credit yet");
          if(credIter->freezed.amount > 0){
               credits.modify(credIter, 0, [&](auto& a){
                         a.money += a.freezed;
                         a.freezed.amount = 0;
                    });
          }          
     }

     // @abi action
     // struct reset{
     //       string confirm, what;
     // };

     // void do_reset(string confirm, string what){
     //      eosio_assert(confirm == "CONFIRM RESET", "must confirm");
     //      require_auth(_self);  // only owner can publish match && game
     //      if(what == "socctrl"){
     //           while(true){
     //                auto iter = socctrls.begin();
     //                if(iter == socctrls.end()){
     //                     break;
     //                }
     //                socctrls.erase(iter);
     //           }
     //      }
     //      if(what == "credit"){
     //           while(true){
     //                auto iter = credits.begin();
     //                if(iter == credits.end()){
     //                     break;
     //                }
     //                credits.erase(iter);
     //           }
     //      }
     //      if(what == "match"){
     //           while(true){
     //                auto iter = matches.begin();
     //                if(iter == matches.end()){
     //                     break;
     //                }
     //                matches.erase(iter);
     //           }
     //      }
     //      if(what == "game"){
     //            uint64_t cnt = 0;
     //           while(cnt++ < 100){
     //                auto iter = games.begin();
     //                if(iter == games.end()){
     //                     break;
     //                }
     //                games.erase(iter);
     //           }
     //      }
     // }
     
     // @abi action
     struct resolve{
          string dummy;
     };
     
     void do_resolve(string dummy){
          require_auth(_self);
          auto ctrlItr = socctrls.begin();
          eosio_assert(ctrlItr != socctrls.end() && !ctrlItr->ext1, "contract freezed"); // ext1 means contractFreezed
          asset stat_outcoming = ctrlItr->stat_outcoming;
          asset stat_credit = ctrlItr->stat_credit;
          asset stat_holding = ctrlItr->stat_holding;
          
          auto freeze_index = credits.template get_index<N(freezed)>();
          while(true){
               auto fIter = freeze_index.lower_bound(1);
               if(fIter == freeze_index.end()){
                    break;
               }

               eosio_assert(fIter->freezed.amount > 0, "freezed not positive");
               auto quantity = fIter->freezed;

               freeze_index.modify(fIter, 0, [&](auto& c){
                         c.freezed.amount = 0;
                    });
               eosio::currency::inline_transfer(_self, fIter->account,
                                                eosio::extended_asset(quantity, N(eosio.token)), "withdraw");
               stat_outcoming += quantity;
               stat_holding -= quantity;
               stat_credit -= quantity;
          }
          eosio_assert((stat_holding.amount == (ctrlItr->stat_incoming.amount - stat_outcoming.amount)) && stat_holding.amount >= 0 , "balance should not <0");
          eosio_assert(stat_holding ==
                       ctrlItr->stat_profit + ctrlItr->stat_bet + stat_credit , "ledger not balance");
          
          socctrls.modify(ctrlItr, 0, [&](auto &s){
                    s.stat_outcoming = stat_outcoming;
                    s.stat_holding = stat_holding;
                    s.stat_credit = stat_credit;
               });
     }

     // @abi action
     struct newmatch{
          time_t tmBegin;
          time_t tmEnd;
          string lTeam;
          string rTeam;
          string desc;
          EOSLIB_SERIALIZE(newmatch, (tmBegin)(tmEnd)(lTeam)(rTeam)(desc))
     };
          
     void do_newmatch(time_t tmBegin, time_t tmEnd, string lTeam, string rTeam, string desc){
          require_auth(_self);  // only owner can publish match && game
          
          auto tmnow = now();
          eosio_assert(tmBegin <= tmnow + 50 * 24 * 60 * 60, "tmBegin should within 7days");
          eosio_assert(tmBegin >= tmnow + 60 * 60, "tmBegin at least 1H after now");
          eosio_assert(tmBegin + 3 * 60 * 60 <= tmEnd, "tmEnd should after tmBegin 3H");
          eosio_assert(lTeam != "", "lTeam empty");
          eosio_assert(rTeam != "", "rTeam empty");
          eosio_assert(lTeam != rTeam, "lTeam and rTeam no same");
               
          // not found, create new match.
          auto ctrlItr = socctrls.begin();
          if( ctrlItr == socctrls.end() ) {
               ctrlItr = socctrls.emplace(_self, [&](auto& a){
                         a.id = 0;
                         a.next_index = 0;
                         a.discnt = DEFAULT_DISCOUNT_RATIO;
                    });
          }
          eosio_assert(ctrlItr != socctrls.end() && !ctrlItr->ext1, "contract freezed"); // ext1 means contractFreezed
          auto nextID = ctrlItr->next_index;

          // no duplicate match
          for(auto matchItr = matches.begin(); matchItr != matches.end(); matchItr ++){
               eosio_assert(matchItr->tmBegin != tmBegin || matchItr->lTeam != lTeam || matchItr->rTeam != rTeam, "match duplicated");
          }          
          
          // setup new matches
          matches.emplace(_self, [&](auto& a){
                    a.matchid = nextID;
                    a.tmBegin = tmBegin;
                    a.tmEnd = tmEnd;
                    a.lTeam = lTeam;
                    a.rTeam = rTeam;
                    a.lScore = -1; // not finished
                    a.rScore = -1; // not finished
                    a.desc = desc;
                    a.status = MATCH_STATUS_OPEN;
               });

          // setup new games
          for(int concede = MIN_CONCEDE; concede <= MAX_CONCEDE; concede ++){
               games.emplace(_self, [&](auto& a){
                         a.gameid = nextID + concede - MIN_CONCEDE + 1;
                         a.matchid = nextID;
                         a.concede = concede;
                         a.win.amount = 0;
                         a.win.symbol = CORE_SYMBOL;
                         a.punch.amount = 0;
                         a.punch.symbol = CORE_SYMBOL;
                         a.lose.amount = 0;
                         a.lose.symbol = CORE_SYMBOL;
                    });
          }

          // update ids
          socctrls.modify(ctrlItr, 0, [&](auto &a){
                    a.next_index += MAX_CONCEDE - MIN_CONCEDE + 1 + 1; // note :: match id consume one
               });
     }

     // @abi action
     struct rmvmatch{
          uint64_t matchid;
          time_t tmBegin;
          string lTeam;
          string rTeam;
          EOSLIB_SERIALIZE(rmvmatch, (matchid)(tmBegin)(lTeam)(rTeam))
     };
     
     void do_rmvmatch(uint64_t matchid, time_t tmBegin, string lTeam, string rTeam){
          require_auth(_self);  // only owner can publish match && game

          auto ctrlItr = socctrls.begin();
          eosio_assert(ctrlItr != socctrls.end() && !ctrlItr->ext1, "contract freezed"); // ext1 means contractFreezed
      
          // find if match is exist or not
          auto matchItr = matches.find(matchid);
          eosio_assert(matchItr != matches.end(), "no such matchid");
          eosio_assert(matchItr->tmBegin == tmBegin, "tmBegin not match");
          eosio_assert(matchItr->lTeam == lTeam, "lTeam not match");
          eosio_assert(matchItr->rTeam == rTeam, "rTeam not match");

          // ensure all game is end
          for(int8_t concede = MIN_CONCEDE; concede <= MAX_CONCEDE; concede ++){
               uint64_t gameid = matchid + 1 + concede - MIN_CONCEDE;
               auto gameItr = games.find(gameid);
               eosio_assert(gameItr == games.end(), "pending game reject remove");
          }

          matches.erase(matchItr);
     }
     
     // @abi action
     struct freeze{
          uint64_t  matchid;    // to compare
          time_t    tmBegin;    // to compare
          string    lTeam;      // to compare
          string    rTeam;      // to compare
          int8_t    lScore;     // to update
          int8_t    rScore;     // to update
          EOSLIB_SERIALIZE(freeze, (matchid)(tmBegin)(lTeam)(rTeam)(lScore)(rScore))
     };
     // just freeze a game not contract
     void do_freeze(uint64_t matchid, time_t tmBegin, string lTeam, string rTeam, int8_t lScore, int8_t rScore){
          require_auth(_self);  // only owner can publish match && game
          auto ctrlItr = socctrls.begin();
          eosio_assert(ctrlItr != socctrls.end() && !ctrlItr->ext1, "contract freezed"); // ext1 means contractFreezed
          // find if match is exist or not
          auto matchItr = matches.find(matchid);

          eosio_assert(matchItr != matches.end(), "no such matchid");
          //eosio::print("begin:", matchItr->tmBegin, ",lteam:", matchItr->lTeam, ", rTeam:", matchItr->rTeam, "\n" );
          eosio_assert(matchItr->tmBegin == tmBegin, "tmBegin not match");
          eosio_assert(matchItr->lTeam == lTeam, "lTeam not match");
          eosio_assert(matchItr->rTeam == rTeam, "rTeam not match");
          
          // found, just update existed match info
          matches.modify(matchItr, 0, [&](auto& a){
                    a.lScore = lScore;
                    a.rScore = rScore;
                    a.status = MATCH_STATUS_FREEZE;
               });               
     }

     // @abi action
     struct update{
          uint64_t  matchid;    // to compare
          time_t    tmBegin;    // to compare
          string    lTeam;      // to compare
          string    rTeam;      // to compare
          string    lTeamNew;   // to update
          string    rTeamNew;   // to update
          EOSLIB_SERIALIZE(update, (matchid)(tmBegin)(lTeam)(rTeam)(lTeamNew)(rTeamNew))
     };          
     // to reset team
     void do_update(uint64_t matchid, time_t tmBegin, string lTeam, string rTeam, string lTeamNew, string rTeamNew){
          require_auth(_self);  // only owner can publish match && game
          auto ctrlItr = socctrls.begin();
          eosio_assert(ctrlItr != socctrls.end() && !ctrlItr->ext1, "contract freezed"); // ext1 means contractFreezed
          // find if match is exist or not
          auto matchItr = matches.find(matchid);
          eosio_assert(matchItr != matches.end(), "no such matchid");
          //eosio::print("begin:", matchItr->tmBegin, ",lteam:", matchItr->lTeam, ", rTeam:", matchItr->rTeam, "\n" );
          eosio_assert(matchItr->tmBegin == tmBegin, "tmBegin not match");
          eosio_assert(matchItr->lTeam == lTeam, "lTeam not match");
          eosio_assert(matchItr->rTeam == rTeam, "rTeam not match");
          eosio_assert(lTeamNew != "", "lTeamNew invalid");
          eosio_assert(rTeamNew != "", "rTeamNew invalid");
          
          // found, just update existed match info
          matches.modify(matchItr, 0, [&](auto& a){
                    a.lTeam = lTeamNew;
                    a.rTeam = rTeamNew;
               });
     }
     
     // @abi action
     struct bet{
          account_name  account;
          uint64_t      gameid;
          asset         money;
          int8_t        side; // 1, bet win; 0, bet punch;  -1 bet lose
          string        lTeam;
          string        rTeam;
          int8_t        concede;
          EOSLIB_SERIALIZE(bet, (account)(gameid)(money)(side)(lTeam)(rTeam)(concede))
     };
     
     void do_bet(account_name account, uint64_t gameid, asset money, int8_t side, string lTeam,
                 string rTeam, int8_t concede){
          //eosio::print("do_bet : gameid ", gameid, "->", eosio::name{gameid},"\n");

          require_auth(account);
          eosio_assert(side == 1 || side == 0 || side == -1, "side must be [-1,0,1]");
          eosio_assert(money.symbol == CORE_SYMBOL, "accept EOS only");
          eosio_assert(money.is_valid(), "bet meony invalid");
          eosio_assert(money.amount == 0 ||
                       (money.amount >= MIN_AMOUNT && money.amount <= MAX_AMOUNT),
                       "amount must 0 or [0.1,1]");

          auto ctrlItr = socctrls.begin();
          eosio_assert(ctrlItr != socctrls.end() && !ctrlItr->ext1, "contract freezed"); // ext1 means contractFreezed
          auto gameItr = games.find(gameid);
          eosio_assert(gameItr != games.end(), "game not existed");
          eosio_assert(concede == gameItr->concede, "concede not match");

          auto matchItr = matches.find(gameItr->matchid);
          eosio_assert(matchItr != matches.end(), "match not existed");
          eosio_assert(matchItr->tmBegin > now() + NOBET_INTERVAL, "game has begin");
          eosio_assert(lTeam == matchItr->lTeam, "lteam not match");
          eosio_assert(rTeam == matchItr->rTeam, "rteam not match");          
          eosio_assert(MATCH_STATUS_OPEN == matchItr->status, "status not betable");

          auto credIter = credits.find(account);
          eosio_assert(credIter != credits.end(), "no credit yet");
          //eosio::print("do bet : use ", gameid, " as scope : ", eosio::name{gameid},"\n");
          betinfo_index_t infos(_self, gameid);
          auto betItr = infos.find(account);

          asset new_credit, new_bet;
          
          if(betItr == infos.end()){  // new bet for this game
               eosio_assert(money.amount != 0, "new bet but amount 0");
               new_bet = ctrlItr->stat_bet + money;
               new_credit = ctrlItr->stat_credit - money;               

               eosio_assert(credIter->money.amount >= money.amount, "credit insufficient @1");
               
               credits.modify(credIter, 0, [&](auto& c){
                         c.money -= money;
                    });
               games.modify(gameItr, 0, [&](auto& g){
                         if(side == 1){
                              eosio_assert(g.win + money > g.win, "overflow on win");
                              g.win += money;
                         }else if (side == 0){
                              eosio_assert(g.punch + money > g.punch, "overflow on punch");
                              g.punch += money;
                         }else{
                              eosio_assert(g.lose + money > g.lose, "overflow on lose");
                              g.lose += money;
                         }
                    });
               
               infos.emplace(_self, [&](auto &a){
                         a.account = account;
                         a.gameid = gameid;
                         a.tmBet = now();
                         a.money = money;
                         a.side = side;
                    });
          }else{// already in game
               new_bet = ctrlItr->stat_bet + money - betItr->money;
               new_credit = ctrlItr->stat_credit + betItr->money - money;

               eosio_assert(betItr->money.amount + credIter->money.amount >= money.amount, "credit insufficient @2");

               games.modify(gameItr, 0, [&](auto& g){
                         // remove old
                         if(betItr->side == 1){
                              g.win -= betItr->money;
                         }else if (betItr->side == 0){
                              g.punch -= betItr->money;
                         }else{
                              g.lose -= betItr->money;
                         }
                         // add as new
                         if(side > 0){
                              g.win += money;
                         }else if (side == 0){
                              g.punch += money;
                         }else{
                              g.lose += money;
                         }
                    });
          
               // change the bet 
               credits.modify(credIter, 0, [&](auto& c){
                         c.money = c.money + betItr->money - money;
                    });

               if(money.amount > 0){
                    infos.modify(betItr, 0, [&](auto &b){
                              b.money = money;
                              b.side = side;
                              b.tmBet = now();
                         });
               }else{
                    infos.erase(betItr);
               }
          }
          // check && update statistics
          eosio_assert(new_credit.amount >= 0 , "new_credit is negative");
          eosio_assert(ctrlItr->stat_holding ==
                       ctrlItr->stat_profit + new_bet + new_credit , "ledger not balance");
          socctrls.modify(ctrlItr, 0, [&](auto &s){
                    s.stat_bet = new_bet;
                    s.stat_credit = new_credit;
               });
     }

     // @abi action
     struct settle{
          uint64_t  gameid;
          string    lTeam;
          string    rTeam;
          int8_t    concede;
          EOSLIB_SERIALIZE(settle, (gameid)(lTeam)(rTeam)(concede));
     };     
          
     void do_settle(uint64_t gameid, string lTeam, string rTeam, int8_t concede){
          asset delta_profit = asset(0, CORE_SYMBOL);
          asset delta_bet = asset(0, CORE_SYMBOL);
          asset delta_credit = asset(0, CORE_SYMBOL);
          
          require_auth(_self);

          auto ctrlItr = socctrls.begin();
          eosio_assert(ctrlItr != socctrls.end() && !ctrlItr->ext1, "contract freezed"); // ext1 means contractFreezed
          
          auto gameItr = games.find(gameid);
          eosio_assert(games.end() != gameItr, "gameid not exist");
          eosio_assert(concede == gameItr->concede, "concede not match");
          
          auto matchItr = matches.find(gameItr->matchid);
          eosio_assert(matches.end() != matchItr, "matchid not exist");
          eosio_assert(matchItr->status == MATCH_STATUS_FREEZE,  "match not freezed");
          eosio_assert(lTeam == matchItr->lTeam, "left team not match");
          eosio_assert(rTeam == matchItr->rTeam, "right team not match");
          //eosio::print("do bet : use ", gameid, " as scope : ", eosio::name{gameid}, "\n");
          betinfo_index_t infos(_self, gameid);
          int8_t score;
          asset base, profit;

          if(matchItr->lScore > matchItr->rScore + concede){ // !! consider concede
               score = 1;
               base = gameItr->win;               
               profit = gameItr->punch + gameItr->lose;
               eosio_assert(profit >= gameItr->punch && profit >= gameItr->lose, "overflow on win");
          }else if(matchItr->lScore == matchItr->rScore + concede){
               score = 0;
               base = gameItr->punch;
               profit = gameItr->win + gameItr->lose;
               eosio_assert(profit >= gameItr->win && profit >= gameItr->lose, "overflow on punch");
          }else {
               score = -1;
               base = gameItr->lose;
               profit = gameItr->win + gameItr->punch;
               eosio_assert(profit >= gameItr->win && profit >= gameItr->punch, "overflow on lose");
          }

          while(true){
               // TODO:: think about settle time out
               auto betItr = infos.begin();
               if(betItr == infos.end()){
                    break;
               }

               delta_bet -= betItr->money;
               if(betItr->side == score){ // for winners
                    delta_credit += betItr->money;

                    auto creditItr = credits.find(betItr->account);
                    eosio_assert(creditItr != credits.end(), "account missing");

                    credits.modify(creditItr, 0, [&](auto& a){
                              a.money.amount += betItr->money.amount;
                              if(base.amount > 0){
                                   a.money.amount += (100-ctrlItr->discnt) * betItr->money.amount * profit.amount / base.amount / 100;
                              }
                         });
               }else{           // for losers
                    delta_credit += betItr->money * (100 - ctrlItr->discnt)/100;
                    delta_profit += betItr->money * ctrlItr->discnt/100;
                    if(profit.amount == 0){ // no opposite bets, return money back.
                         delta_credit += betItr->money;                         
                    }
               }               
               
               infos.erase(betItr);
          }

          eosio_assert(0 == delta_bet.amount + delta_profit.amount + delta_credit.amount, "delta not balance");

          socctrls.modify(ctrlItr, 0, [&](auto &s){
                    s.stat_bet += delta_bet;
                    s.stat_credit += delta_credit;
                    s.stat_profit += delta_profit;
               });
          //eosio::print("game settled\n");
          games.erase(gameItr);
     }

     // @abi action
     struct adjust{
          uint8_t discnt;
          uint8_t ext1, ext2, ext3;
          uint64_t ext4;
          EOSLIB_SERIALIZE(adjust, (discnt)(ext1)(ext2)(ext3)(ext4));
     };
     
     void do_adjust(uint8_t discnt, uint8_t ext1, uint8_t ext2, uint8_t ext3, uint64_t ext4){
          require_auth(_self);
          eosio_assert(discnt <= MAX_DISCOUNT_RATIO, "discount too high");
          auto ctrlItr = socctrls.begin();
          if( ctrlItr == socctrls.end() ) {
               ctrlItr = socctrls.emplace(_self, [&](auto& a){
                         a.id = 0;
                         a.next_index = 0;
                         a.discnt = discnt;
                         a.ext1 = ext1;
                         a.ext2 = ext2;
                         a.ext3 = ext3;
                         a.ext4 = ext4;
                    });
               return;
          }
          socctrls.modify(ctrlItr, 0, [&](auto& a){
                    a.discnt = discnt;
                    a.ext1 = ext1;
                    a.ext2 = ext2;
                    a.ext3 = ext3;
                    a.ext4 = ext4;
               });
     }     
     
     // @abi action
     struct cancel{
          uint64_t  gameid;
          string    lTeam;
          string    rTeam;
          int8_t    concede;
          EOSLIB_SERIALIZE(cancel, (gameid)(lTeam)(rTeam)(concede));
     };     
     // cancel a game
     void do_cancel(uint64_t gameid, string lTeam, string rTeam, int8_t concede){
          require_auth(_self);

          auto ctrlItr = socctrls.begin();
          eosio_assert(ctrlItr != socctrls.end() && !ctrlItr->ext1, "contract freezed"); // ext1 means contractFreezed
          
          auto gameItr = games.find(gameid);
          eosio_assert(games.end() != gameItr, "gameid not exist");
          eosio_assert(concede == gameItr->concede, "concede not match");
          
          auto matchItr = matches.find(gameItr->matchid);
          eosio_assert(matches.end() != matchItr, "matchid not exist");
          eosio_assert(matchItr->status != MATCH_STATUS_OPEN, "freeze first");
          eosio_assert(lTeam == matchItr->lTeam, "left team not match");
          eosio_assert(rTeam == matchItr->rTeam, "right team not match");
          //eosio::print("do bet : use ", gameid, " as scope : ", eosio::name{gameid}, "\n");
          betinfo_index_t infos(_self, gameid);
          uint64_t win = gameItr->win.amount;
          uint64_t punch = gameItr->punch.amount;
          uint64_t lose = gameItr->lose.amount;

          asset delta_bet = asset(0, CORE_SYMBOL), delta_credit = asset(0, CORE_SYMBOL);
          
          while(true){
               auto betItr = infos.begin();
               if(betItr == infos.end()){
                    break;
               }

               auto creditItr = credits.find(betItr->account);
               eosio_assert(creditItr != credits.end(), "account missing");

               if(betItr->side > 0){
                    win -= betItr->money.amount;
               }else if(betItr->side == 0){
                    punch -= betItr->money.amount;
               }else{
                    lose -= betItr->money.amount;
               }

               delta_bet -= betItr->money;
               delta_credit += betItr->money;
               credits.modify(creditItr, 0, [&](auto& a){
                         a.money += betItr->money;
                    });

               infos.erase(betItr);
          }

          eosio_assert(win == 0, "win is not 0");
          eosio_assert(punch == 0, "punch is not 0");
          eosio_assert(lose == 0, "lose is not 0");

          eosio_assert(ctrlItr->stat_bet.amount + delta_bet.amount >= 0, "bet not balance");
          socctrls.modify(ctrlItr, 0, [&](auto &s){
                    s.stat_bet += delta_bet;
                    s.stat_credit += delta_credit;
               });
          //eosio::print("game ", gameItr->gameid, " canceled\n");
          games.erase(gameItr);
     }

     void apply(account_name code, account_name action) {
          if(code != _self){   // notification handlers, soccer is not action target, just be notified
               switch(action){
               case N(transfer):
                    return transfer(code, eosio::unpack_action_data<eosio::currency::transfer>());
               default:
                    //eosio::print("ignore invalid notification source : ", eosio::name{code}, ", action : ", eosio::name{action},"\n");
                    return;
               }               
          }

          // case we are the action target
          switch(action){
               // case N(reset):{
               //      auto r = eosio::unpack_action_data<reset>();
               //      do_reset(r.confirm, r.what);
               //      return;
               // }
          case N(withdraw):{
               auto w = eosio::unpack_action_data<withdraw>();
               do_withdraw(w.account, w.quantity);
               return;
          }
          case N(cancelwd):{
               auto c = eosio::unpack_action_data<cancelwd>();
               do_cancelwd(c.account);
               return;
          }          
          case N(resolve):{
               do_resolve("");
               return;
          }          
          case N(newmatch):{
               auto n = eosio::unpack_action_data<newmatch>();
               do_newmatch(n.tmBegin, n.tmEnd, n.lTeam, n.rTeam, n.desc);
               return;
          }
          case N(rmvmatch):{
               auto r = eosio::unpack_action_data<rmvmatch>();
               do_rmvmatch(r.matchid, r.tmBegin, r.lTeam, r.rTeam);
               return;
          }          
          case N(freeze):{
               auto f = eosio::unpack_action_data<freeze>();
               do_freeze(f.matchid, f.tmBegin, f.lTeam, f.rTeam, f.lScore, f.rScore);
               return;
          }
          case N(update):{
               auto u = eosio::unpack_action_data<update>();
               do_update(u.matchid, u.tmBegin, u.lTeam, u.rTeam, u.lTeamNew, u.rTeamNew);
               return;
          }
          case N(bet):{
               auto b = eosio::unpack_action_data<bet>();
               do_bet(b.account, b.gameid, b.money, b.side, b.lTeam, b.rTeam, b.concede);
               return;
          }
          case N(settle):{
               auto s = eosio::unpack_action_data<settle>();
               do_settle(s.gameid, s.lTeam, s.rTeam, s.concede);
               return;
          }
          case N(adjust):{
               auto a = eosio::unpack_action_data<adjust>();
               do_adjust(a.discnt, a.ext1, a.ext2, a.ext3, a.ext4);
               return;
          }
          case N(cancel):{
               auto c = eosio::unpack_action_data<cancel>();
               do_cancel(c.gameid, c.lTeam, c.rTeam, c.concede);
               return;
          }          
          default:
               //eosio::print("ignore unknown action : ", eosio::name{action}, "\n");
               eosio_assert(false, "unknown action");
          }
     }
     
private:
     // @abi table
     struct credit {
          account_name      account;
          asset             money;
          asset             freezed;
          uint64_t          winCnt, loseCnt; // TODO:: process it
          asset             winMoney, loseMoney; // TODO:: process it
          uint64_t primary_key() const {return account;}
          uint64_t get_freezed_amount() const {return freezed.amount;}
          
          EOSLIB_SERIALIZE(credit, (account)(money)(freezed)(winCnt)(loseCnt)(winMoney)(loseMoney));
     };
     typedef eosio::multi_index<N(credit), credit,
                                eosio::indexed_by< N(freezed), eosio::const_mem_fun<credit, uint64_t, &credit::get_freezed_amount> > > credit_index_t;
     credit_index_t credits;

     // @abi table
     struct socctrl{
          uint64_t          id = 0;
          uint64_t          next_index = 0;
          uint8_t           discnt = DEFAULT_DISCOUNT_RATIO; // in x%, percentage
          uint8_t           ext1 = 0, ext2 = 0, ext3 = 0; // take ext1 as freezeContract by now
          uint64_t          ext4 = 0;
          asset             stat_incoming;  // all incoming statistics
          asset             stat_outcoming; // all oucomint statistics
          asset             stat_holding; // holding statistics
          asset             stat_profit; // profit of mine
          asset             stat_credit; // all credit's freezed + money
          asset             stat_bet; // all in bet
          // incoming - outcoming == profit + credit + bet
          uint64_t primary_key() const {return id;}
          EOSLIB_SERIALIZE(socctrl, (id)(next_index)(discnt)(ext1)(ext2)(ext3)(ext4)(stat_incoming)(stat_outcoming)(stat_holding)(stat_profit)(stat_credit)(stat_bet))
     };
     typedef eosio::multi_index<N(socctrl), socctrl> socctrl_index_t;
     socctrl_index_t socctrls;     

     //@abi table
     struct match{              // 一局比赛,对应[-5:+5]11个赌局
          uint64_t          matchid;
          time_t            tmBegin, tmEnd;
          string            lTeam, rTeam;
          string            desc;
          int8_t            lScore = -1, rScore = -1; // < 0 means not finished
          int8_t            status; // 1, open for bet; 0, freeze; -1, game over, closed;
          uint64_t primary_key() const {
               // TODO:: how about issue an action at here? secure?
               return matchid;
          }
          EOSLIB_SERIALIZE(match, (matchid)(tmBegin)(tmEnd)(lTeam)(rTeam)(desc)(lScore)(rScore)(status))
     };
     typedef eosio::multi_index<N(match), match> match_index_t;
     match_index_t matches;
     
     // @abi table
     struct game{               // 一场比赛的某一个赌局
          uint64_t          gameid;
          uint64_t          matchid;
          int8_t            concede;
          asset             win;
          asset             punch;
          asset             lose;
          uint64_t primary_key() const {return gameid;}
          EOSLIB_SERIALIZE(game, (gameid)(matchid)(concede)(win)(punch)(lose))
     };
     typedef eosio::multi_index<N(game), game> game_index_t;
     game_index_t games;

     // @abi table
     struct betinfo{           // 某个用户押注情况, gameid 作为scope
          account_name      account;
          uint64_t          gameid;
          time_t            tmBet;
          asset             money;
          int8_t            side; // 1, bet left win;0, bet punch;-1 bet right win.
          uint64_t primary_key() const {return account;}
          EOSLIB_SERIALIZE(betinfo, (account)(gameid)(tmBet)(money)(side))
     };
     typedef eosio::multi_index<N(betinfo), betinfo> betinfo_index_t;
};

extern "C" {
     void apply(uint64_t receiver, uint64_t code, uint64_t action) {
          soccer s(receiver);
          s.apply(code, action);
     }
}
