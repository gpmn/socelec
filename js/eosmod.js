// This file is required by the index.html file and will
// be executed in the renderer process for that window.
// All of the Node.js APIs are available in this process.

var allTeams = new Map();
allTeams.set("Argentina",   "阿根廷");
allTeams.set("Australia",   "澳大利亚");
allTeams.set("Belgium",     "比利时");
allTeams.set("Brazil",      "巴西");
allTeams.set("Colombia",    "哥伦比亚");
allTeams.set("CostaRica",   "澳大利亚");
allTeams.set("Croatia",     "克罗地亚");
allTeams.set("Denmark",     "丹麦");
allTeams.set("Egypt",       "埃及");
allTeams.set("England",     "英格兰");
allTeams.set("France",      "法国");
allTeams.set("Germany",     "德国");
allTeams.set("Iceland",     "冰岛");
allTeams.set("Iran",        "伊朗");
allTeams.set("Japan",       "日本");
allTeams.set("Mexico",      "墨西哥");
allTeams.set("Morocco",     "摩洛哥");
allTeams.set("Nigeria",     "尼日利亚");
allTeams.set("Panama",      "巴拿马");
allTeams.set("Peru",        "秘鲁");
allTeams.set("Poland",      "波兰");
allTeams.set("Portugal",    "葡萄牙");
allTeams.set("Russia",      "俄罗斯");
allTeams.set("SaudiArabia", "沙特阿拉伯");
allTeams.set("Senegal",     "塞内加尔");
allTeams.set("Serbia",      "塞尔维亚");
allTeams.set("Korea",       "韩国");
allTeams.set("Spain",       "西班牙");
allTeams.set("Sweden",      "瑞典");
allTeams.set("Switzerland", "瑞士");
allTeams.set("Tunisia",     "突尼斯");
allTeams.set("Uruguay",     "乌拉圭");

/* var allMatches = [
 *     {id:1,  tm:"2018-06-14T23:00+08:00", desc:"FIFA2018 A group",      lteam:"Russia",         rteam:"SaudiArabia"},
 *     {id:2,  tm:"2018-06-15T20:00+08:00", desc:"FIFA2018 A group",      lteam:"Egypt",          rteam:"Uruguay"},
 *     {id:3,  tm:"2018-06-15T23:00+08:00", desc:"FIFA2018 B group",      lteam:"Morocco",        rteam:"Iran"},
 *     {id:4,  tm:"2018-06-16T02:00+08:00", desc:"FIFA2018 B group",      lteam:"Portugal",       rteam:"Spain"},
 *     {id:5,  tm:"2018-06-16T18:00+08:00", desc:"FIFA2018 C group",      lteam:"France",         rteam:"Australia"},
 *     {id:6,  tm:"2018-06-16T21:00+08:00", desc:"FIFA2018 D group",      lteam:"Argentina",      rteam:"Iceland"},
 *     {id:7,  tm:"2018-06-17T00:00+08:00", desc:"FIFA2018 C group",      lteam:"Peru",           rteam:"Denmark"},
 *     {id:8,  tm:"2018-06-17T03:00+08:00", desc:"FIFA2018 D group",      lteam:"Croatia",        rteam:"Nigeria"},
 *     {id:9,  tm:"2018-06-17T20:00+08:00", desc:"FIFA2018 E group",      lteam:"CostaRica",      rteam:"Serbia"},
 *     {id:10, tm:"2018-06-17T23:00+08:00", desc:"FIFA2018 F group",      lteam:"Germany",        rteam:"Mexico"},
 *     {id:11, tm:"2018-06-18T02:00+08:00", desc:"FIFA2018 E group",      lteam:"Brazil",         rteam:"Switzerland"},
 *     {id:12, tm:"2018-06-18T20:00+08:00", desc:"FIFA2018 F group",      lteam:"Sweden",         rteam:"Korea"},
 *     {id:13, tm:"2018-06-18T23:00+08:00", desc:"FIFA2018 G group",      lteam:"Belgium",        rteam:"Panama"},
 *     {id:14, tm:"2018-06-19T02:00+08:00", desc:"FIFA2018 G group",      lteam:"Tunisia",        rteam:"England"},
 *     {id:15, tm:"2018-06-19T20:00+08:00", desc:"FIFA2018 H group",      lteam:"Colombia",       rteam:"Japan"},
 *     {id:16, tm:"2018-06-19T23:00+08:00", desc:"FIFA2018 H group",      lteam:"Poland",         rteam:"Senegal"},
 *     {id:17, tm:"2018-06-20T02:00+08:00", desc:"FIFA2018 A group",      lteam:"Russia",         rteam:"Egypt"},
 *     {id:18, tm:"2018-06-20T20:00+08:00", desc:"FIFA2018 B group",      lteam:"Portugal",       rteam:"Morocco"},
 *     {id:19, tm:"2018-06-20T23:00+08:00", desc:"FIFA2018 A group",      lteam:"Uruguay",        rteam:"SaudiArabia"},
 *     {id:20, tm:"2018-06-21T02:00+08:00", desc:"FIFA2018 B group",      lteam:"Iran",           rteam:"Spain"},
 *     {id:21, tm:"2018-06-21T20:00+08:00", desc:"FIFA2018 C group",      lteam:"Denmark",        rteam:"Australia"},
 *     {id:22, tm:"2018-06-21T23:00+08:00", desc:"FIFA2018 C group",      lteam:"France",         rteam:"Peru"},
 *     {id:23, tm:"2018-06-22T02:00+08:00", desc:"FIFA2018 D group",      lteam:"Argentina",      rteam:"Croatia"},
 *     {id:24, tm:"2018-06-22T20:00+08:00", desc:"FIFA2018 E group",      lteam:"Brazil",         rteam:"CostaRica"},
 *     {id:25, tm:"2018-06-22T23:00+08:00", desc:"FIFA2018 D group",      lteam:"Nigeria",        rteam:"Iceland"},
 *     {id:26, tm:"2018-06-23T02:00+08:00", desc:"FIFA2018 E group",      lteam:"Serbia",         rteam:"Switzerland"},
 *     {id:27, tm:"2018-06-23T20:00+08:00", desc:"FIFA2018 G group",      lteam:"Belgium",        rteam:"Tunisia"},
 *     {id:28, tm:"2018-06-23T23:00+08:00", desc:"FIFA2018 F group",      lteam:"Korea",          rteam:"Mexico"},
 *     {id:29, tm:"2018-06-24T02:00+08:00", desc:"FIFA2018 F group",      lteam:"Germany",        rteam:"Sweden"},
 *     {id:30, tm:"2018-06-24T20:00+08:00", desc:"FIFA2018 G group",      lteam:"England",        rteam:"Panama"},
 *     {id:31, tm:"2018-06-24T23:00+08:00", desc:"FIFA2018 H group",      lteam:"Japan",          rteam:"Senegal"},
 *     {id:32, tm:"2018-06-25T02:00+08:00", desc:"FIFA2018 H group",      lteam:"Poland",         rteam:"Colombia"},
 *     {id:33, tm:"2018-06-25T22:00+08:00", desc:"FIFA2018 A group",      lteam:"Uruguay",        rteam:"Russia"},
 *     {id:34, tm:"2018-06-25T22:00+08:00", desc:"FIFA2018 A group",      lteam:"SaudiArabia",    rteam:"Egypt"},
 *     {id:35, tm:"2018-06-26T02:00+08:00", desc:"FIFA2018 B group",      lteam:"Iran",           rteam:"Portugal"},
 *     {id:36, tm:"2018-06-26T02:00+08:00", desc:"FIFA2018 B group",      lteam:"Spain",          rteam:"Morocco"},
 *     {id:37, tm:"2018-06-26T22:00+08:00", desc:"FIFA2018 C group",      lteam:"Denmark",        rteam:"France"},
 *     {id:38, tm:"2018-06-26T22:00+08:00", desc:"FIFA2018 C group",      lteam:"Australia",      rteam:"Peru"},
 *     {id:39, tm:"2018-06-27T02:00+08:00", desc:"FIFA2018 D group",      lteam:"Nigeria",        rteam:"Argentina"},
 *     {id:40, tm:"2018-06-27T02:00+08:00", desc:"FIFA2018 D group",      lteam:"Iceland",        rteam:"Croatia"},
 *     {id:41, tm:"2018-06-27T22:00+08:00", desc:"FIFA2018 F group",      lteam:"Mexico",         rteam:"Sweden"},
 *     {id:42, tm:"2018-06-27T22:00+08:00", desc:"FIFA2018 F group",      lteam:"Korea",          rteam:"Germany"},
 *     {id:43, tm:"2018-06-28T02:00+08:00", desc:"FIFA2018 E group",      lteam:"Serbia",         rteam:"Brazil"},
 *     {id:44, tm:"2018-06-28T02:00+08:00", desc:"FIFA2018 E group",      lteam:" Switzerland",   rteam:"CostaRica"},
 *     {id:45, tm:"2018-06-28T22:00+08:00", desc:"FIFA2018 H group",      lteam:"Japan",          rteam:"Poland"},
 *     {id:46, tm:"2018-06-28T22:00+08:00", desc:"FIFA2018 H group",      lteam:"Senegal",        rteam:"Colombia"},
 *     {id:47, tm:"2018-06-29T02:00+08:00", desc:"FIFA2018 G group",      lteam:"Panama",         rteam:"Tunisia"},
 *     {id:48, tm:"2018-06-29T02:00+08:00", desc:"FIFA2018 G group",      lteam:"England",        rteam:"Belgium"},
 *     {id:49, tm:"2018-06-30T22:00+08:00", desc:"FIFA2018 1/8 final",        lteam:"C1",             rteam:"D2"},
 *     {id:50, tm:"2018-07-01T02:00+08:00", desc:"FIFA2018 1/8 final",        lteam:"A1",             rteam:"B2"},
 *     {id:51, tm:"2018-07-01T22:00+08:00", desc:"FIFA2018 1/8 final",        lteam:"B1",             rteam:"A2"},
 *     {id:52, tm:"2018-07-02T02:00+08:00", desc:"FIFA2018 1/8 final",        lteam:"D1",             rteam:"C2"},
 *     {id:53, tm:"2018-07-02T22:00+08:00", desc:"FIFA2018 1/8 final",        lteam:"E1",             rteam:"F2"},
 *     {id:54, tm:"2018-07-03T02:00+08:00", desc:"FIFA2018 1/8 final",        lteam:"G1",             rteam:"H2"},
 *     {id:55, tm:"2018-07-03T22:00+08:00", desc:"FIFA2018 1/8 final",        lteam:"F1",             rteam:"E2"},
 *     {id:56, tm:"2018-07-04T02:00+08:00", desc:"FIFA2018 1/8 final",        lteam:"H1",             rteam:"G2"},
 *     {id:57, tm:"2018-07-06T22:00+08:00", desc:"FIFA2018 1/4 final",       lteam:"49 Win",         rteam:"50 Win"},
 *     {id:58, tm:"2018-07-07T02:00+08:00", desc:"FIFA2018 1/4 final",       lteam:"53 Win",         rteam:"54 Win"},
 *     {id:59, tm:"2018-07-07T22:00+08:00", desc:"FIFA2018 1/4 final",       lteam:"51 Win",         rteam:"52 Win"},
 *     {id:60, tm:"2018-07-08T02:00+08:00", desc:"FIFA2018 1/4 final",       lteam:"55 Win",         rteam:"56 Win"},
 *     {id:61, tm:"2018-07-11T02:00+08:00", desc:"FIFA2018 1/2 final",         lteam:"57 Win",         rteam:"58 Win"},
 *     {id:62, tm:"2018-07-12T02:00+08:00", desc:"FIFA2018 1/2 final",         lteam:"59 Win",         rteam:"60 Win"},
 *     {id:63, tm:"2018-07-14T22:00+08:00", desc:"FIFA2018 bronze final",       lteam:"61 Lose",        rteam:"62 Lose"},
 *     {id:64, tm:"2018-07-15T23:00+08:00", desc:"FIFA2018 golden final",       lteam:"61 Win",         rteam:"62 Win"}
 * ];*/

function loadMatches(eosAddr){
    var options = {
        httpEndpoint   : eosAddr,
        expireInSeconds: 60,
        broadcast      : true,
        debug          : false,
        sign           : false
    };

    var eos = Eos(options);
    return eos.getTableRows({
        code:"soccer",
        scope:"soccer",
        table:"match",
        limit:256,
        json:true,
    });
}

function loadGames(eosAddr, matchid){
    var options = {
        httpEndpoint   : eosAddr,
        expireInSeconds: 60,
        broadcast      : true,
        debug          : false,
        sign           : false
    };

    var eos = Eos(options);
    return eos.getTableRows({
        code:"soccer",
        scope:"soccer",
        table:"game",
        limit:20,
        lower_bound:(matchid+1).toString(),
        upper_bound:(matchid+12).toString(),
        json:true
    });
}

function loadUserBet(eosAddr, matchid, account){
    var options = {
        httpEndpoint   : eosAddr,
        expireInSeconds: 60,
        broadcast      : true,
        debug          : false,
        sign           : false
    };
    var eos = Eos(options);
    var allPm = [];
    for(var index = 1; index <= 11; index ++){
        var gameid = matchid + index;
        var pm = eos.getTableRows({
            code: "soccer",
            scope: Eos.modules.format.decodeName(gameid, false),
            table: "betinfo",
            limit: 1,
            lower_bound: Eos.modules.format.encodeName(account, false),
            json: true
        });
        allPm.push(pm);
    }
    return Promise.all(allPm)
}

function getCredit(eosAddr, account){
    var options = {
        httpEndpoint   : eosAddr,
        expireInSeconds: 60,
        broadcast      : true,
        debug          : false,
        sign           : false
    };

    var eos = Eos(options);
    return eos.getTableRows({
        code:"soccer",
        scope:"soccer",
        table:"credit",
        lower_bound:Eos.modules.format.encodeName(account, false),
        limit:1,
        json:true
    });    
}

function submitBet(eosAddr, account, activeKey, gameid, money, side, lTeam, rTeam, concede){
    var options = {
        httpEndpoint   : eosAddr,
        keyProvider    : [activeKey],
        expireInSeconds: 60,
        broadcast      : true,
        debug          : false,        
        sign           : true
    };

    var eos = Eos(options);
    return eos.transaction({
        actions: [
            {
                account: 'soccer',
                name: 'bet',
                authorization: [{
                    actor: account,
                    permission: 'active'
                }],
                data: {
                    account: account,
                    gameid: gameid,
                    money: money,
                    side: side,
                    lTeam: lTeam,
                    rTeam: rTeam,
                    concede: concede
                }
            }
        ]
    })
}
