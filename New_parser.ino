#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <WebServer.h>
#include <UniversalTelegramBot.h>
#include <ArduinoJson.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// Данные сети Wi-Fi
const char* ssid = "";
const char* password = "";

// Инициализация веб-сервера на порту 80
WebServer server(80);

// Переменные для хранения введенных данных
String wifiSSID = "";
String wifiPassword = "";
String userID = "";

// Telegram Bot Token
const char* BOTtoken = "7387951016:AAGeH3zvW02BAwWTKkC9xEyJsqbKdf_xAE0";
String CHAT_ID = "1447765166";

// Сервер Telegram
WiFiClientSecure client;
UniversalTelegramBot bot(BOTtoken, client);

// Переменные для таймера
unsigned long lastTimeBotRan = 0;
const long interval = 120000; // Интервал в миллисекундах (30 секунд)
long int t; // Обновление информации

// Сервер CoinGecko
const char* host = "api.coingecko.com";
const int httpsPort = 443; // Порт HTTPS
const int coin_num = 300;

// Создание объекта для дисплея
Adafruit_SSD1306 display(128, 64, &Wire, -1);

// Для данных на экране
String Price_display;
float DayPrice_display;
bool CoinSearch;
String URL = "Undefined";
String Ticker = "Undefined";
bool price_update = false;

// Для работы экрана
bool SleepMode = false;
bool HypersleepMode = false;
int precision_change = 4;
String CurrencyRate = "USD";
String SymbolCoin = "$";
int TickerSize = 2;
int PriceSize = 3;

// Для оповещений
bool update = false;
bool UpOrDown;
float priceUpAlert = -1;  // Цена для оповещения о повышении
float priceDownAlert = -1;  // Цена для оповещения о понижении
String priceUpAlertStr;
String priceDownAlertStr;
float coin;
bool vol = true;

// Структура массива для словаря
struct CoinData {
  String coinName;
  String url;
  String abbreviation;
};

// Инициализация массива с данными о монетах
CoinData coinDict[coin_num] = {
  {"BITCOIN", "/api/v3/simple/price?ids=bitcoin&vs_currencies=%CURRENCY%&include_24hr_change=true", "BTC"},
  {"ETHEREUM", "/api/v3/simple/price?ids=ethereum&vs_currencies=%CURRENCY%&include_24hr_change=true", "ETH"},
  {"TETHER", "/api/v3/simple/price?ids=tether&vs_currencies=%CURRENCY%&include_24hr_change=true", "USDT"},
  {"BNB", "/api/v3/simple/price?ids=binancecoin&vs_currencies=%CURRENCY%&include_24hr_change=true", "BNB"},
  {"SOLANA", "/api/v3/simple/price?ids=solana&vs_currencies=%CURRENCY%&include_24hr_change=true", "SOL"},
  {"USDC", "/api/v3/simple/price?ids=usd-coin&vs_currencies=%CURRENCY%&include_24hr_change=true", "USDC"},
  {"XRP", "/api/v3/simple/price?ids=ripple&vs_currencies=%CURRENCY%&include_24hr_change=true", "XRP"},
  {"LIDO STAKED ETHER", "/api/v3/simple/price?ids=staked-ether&vs_currencies=%CURRENCY%&include_24hr_change=true", "STETH"},
  {"DOGECOIN", "/api/v3/simple/price?ids=dogecoin&vs_currencies=%CURRENCY%&include_24hr_change=true", "DOGE"},
  {"TONCOIN", "/api/v3/simple/price?ids=the-open-network&vs_currencies=%CURRENCY%&include_24hr_change=true", "TON"},
  {"CARDANO", "/api/v3/simple/price?ids=cardano&vs_currencies=%CURRENCY%&include_24hr_change=true", "ADA"},
  {"TRON", "/api/v3/simple/price?ids=tron&vs_currencies=%CURRENCY%&include_24hr_change=true", "TRX"},
  {"WRAPPED BITCOIN", "/api/v3/simple/price?ids=wrapped-bitcoin&vs_currencies=%CURRENCY%&include_24hr_change=true", "WBTC"},
  {"AVALANCHE", "/api/v3/simple/price?ids=avalanche-2&vs_currencies=%CURRENCY%&include_24hr_change=true", "AVAX"},
  {"SHIBA INU", "/api/v3/simple/price?ids=shiba-inu&vs_currencies=%CURRENCY%&include_24hr_change=true", "SHIB"},
  {"POLKADOT", "/api/v3/simple/price?ids=polkadot&vs_currencies=%CURRENCY%&include_24hr_change=true", "DOT"},
  {"BITCOIN CASH", "/api/v3/simple/price?ids=bitcoin-cash&vs_currencies=%CURRENCY%&include_24hr_change=true", "BCH"},
  {"CHAINLINK", "/api/v3/simple/price?ids=chainlink&vs_currencies=%CURRENCY%&include_24hr_change=true", "LINK"},
  {"LEO TOKEN", "/api/v3/simple/price?ids=leo-token&vs_currencies=%CURRENCY%&include_24hr_change=true", "LEO"},
  {"DAI", "/api/v3/simple/price?ids=dai&vs_currencies=%CURRENCY%&include_24hr_change=true", "DAI"},
  {"UNISWAP", "/api/v3/simple/price?ids=uniswap&vs_currencies=%CURRENCY%&include_24hr_change=true", "UNI"},
  {"LITECOIN", "/api/v3/simple/price?ids=litecoin&vs_currencies=%CURRENCY%&include_24hr_change=true", "LTC"},
  {"NEAR PROTOCOL", "/api/v3/simple/price?ids=near&vs_currencies=%CURRENCY%&include_24hr_change=true", "NEAR"},
  {"KASPA", "/api/v3/simple/price?ids=kaspa&vs_currencies=%CURRENCY%&include_24hr_change=true", "KAS"},
  {"POLYGON", "/api/v3/simple/price?ids=matic-network&vs_currencies=%CURRENCY%&include_24hr_change=true", "MATIC"},
  {"WRAPPED EETH", "/api/v3/simple/price?ids=wrapped-eeth&vs_currencies=%CURRENCY%&include_24hr_change=true", "WEETH"},
  {"INTERNET COMPUTER", "/api/v3/simple/price?ids=internet-computer&vs_currencies=%CURRENCY%&include_24hr_change=true", "ICP"},
  {"PEPE", "/api/v3/simple/price?ids=pepe&vs_currencies=%CURRENCY%&include_24hr_change=true", "PEPE"},
  {"ETHENA USDE", "/api/v3/simple/price?ids=ethena-usde&vs_currencies=%CURRENCY%&include_24hr_change=true", "USDE"},
  {"STELLAR", "/api/v3/simple/price?ids=stellar&vs_currencies=%CURRENCY%&include_24hr_change=true", "XLM"},
  {"MONERO", "/api/v3/simple/price?ids=monero&vs_currencies=%CURRENCY%&include_24hr_change=true", "XMR"},
  {"ETHEREUM CLASSIC", "/api/v3/simple/price?ids=ethereum-classic&vs_currencies=%CURRENCY%&include_24hr_change=true", "ETC"},
  {"APTOS", "/api/v3/simple/price?ids=aptos&vs_currencies=%CURRENCY%&include_24hr_change=true", "APT"},
  {"ARTIFICIAL SUPERINTELLIGENCE ALLIANCE", "/api/v3/simple/price?ids=fetch-ai&vs_currencies=%CURRENCY%&include_24hr_change=true", "FET"},
  {"CRONOS", "/api/v3/simple/price?ids=crypto-com-chain&vs_currencies=%CURRENCY%&include_24hr_change=true", "CRO"},
  {"OKB", "/api/v3/simple/price?ids=okb&vs_currencies=%CURRENCY%&include_24hr_change=true", "OKB"},
  {"STACKS", "/api/v3/simple/price?ids=blockstack&vs_currencies=%CURRENCY%&include_24hr_change=true", "STX"},
  {"FILECOIN", "/api/v3/simple/price?ids=filecoin&vs_currencies=%CURRENCY%&include_24hr_change=true", "FIL"},
  {"HEDERA", "/api/v3/simple/price?ids=hedera-hashgraph&vs_currencies=%CURRENCY%&include_24hr_change=true", "HBAR"},
  {"FIRST DIGITAL USD", "/api/v3/simple/price?ids=first-digital-usd&vs_currencies=%CURRENCY%&include_24hr_change=true", "FDUSD"},
  {"MANTLE", "/api/v3/simple/price?ids=mantle&vs_currencies=%CURRENCY%&include_24hr_change=true", "MNT"},
  {"COSMOS HUB", "/api/v3/simple/price?ids=cosmos&vs_currencies=%CURRENCY%&include_24hr_change=true", "ATOM"},
  {"RENDER", "/api/v3/simple/price?ids=render-token&vs_currencies=%CURRENCY%&include_24hr_change=true", "RENDER"},
  {"BITTENSOR", "/api/v3/simple/price?ids=bittensor&vs_currencies=%CURRENCY%&include_24hr_change=true", "TAO"},
  {"VECHAIN", "/api/v3/simple/price?ids=vechain&vs_currencies=%CURRENCY%&include_24hr_change=true", "VET"},
  {"IMMUTABLE", "/api/v3/simple/price?ids=immutable-x&vs_currencies=%CURRENCY%&include_24hr_change=true", "IMX"},
  {"MAKER", "/api/v3/simple/price?ids=maker&vs_currencies=%CURRENCY%&include_24hr_change=true", "MKR"},
  {"DOGWIFHAT", "/api/v3/simple/price?ids=dogwifcoin&vs_currencies=%CURRENCY%&include_24hr_change=true", "WIF"},
  {"SUI", "/api/v3/simple/price?ids=sui&vs_currencies=%CURRENCY%&include_24hr_change=true", "SUI"},
  {"ARBITRUM", "/api/v3/simple/price?ids=arbitrum&vs_currencies=%CURRENCY%&include_24hr_change=true", "ARB"},
  {"AAVE", "/api/v3/simple/price?ids=aave&vs_currencies=%CURRENCY%&include_24hr_change=true", "AAVE"},
  {"INJECTIVE", "/api/v3/simple/price?ids=injective-protocol&vs_currencies=%CURRENCY%&include_24hr_change=true", "INJ"},
  {"OPTIMISM", "/api/v3/simple/price?ids=optimism&vs_currencies=%CURRENCY%&include_24hr_change=true", "OP"},
  {"WHITEBIT COIN", "/api/v3/simple/price?ids=whitebit&vs_currencies=%CURRENCY%&include_24hr_change=true", "WBT"},
  {"BONK", "/api/v3/simple/price?ids=bonk&vs_currencies=%CURRENCY%&include_24hr_change=true", "BONK"},
  {"ROCKET POOL ETH", "/api/v3/simple/price?ids=rocket-pool-eth&vs_currencies=%CURRENCY%&include_24hr_change=true", "RETH"},
  {"ARWEAVE", "/api/v3/simple/price?ids=arweave&vs_currencies=%CURRENCY%&include_24hr_change=true", "AR"},
  {"THE GRAPH", "/api/v3/simple/price?ids=the-graph&vs_currencies=%CURRENCY%&include_24hr_change=true", "GRT"},
  {"BITGET TOKEN", "/api/v3/simple/price?ids=bitget-token&vs_currencies=%CURRENCY%&include_24hr_change=true", "BGB"},
  {"FLOKI", "/api/v3/simple/price?ids=floki&vs_currencies=%CURRENCY%&include_24hr_change=true", "FLOKI"},
  {"RENZO RESTAKED ETH", "/api/v3/simple/price?ids=renzo-restaked-eth&vs_currencies=%CURRENCY%&include_24hr_change=true", "EZETH"},
  {"MANTLE STAKED ETHER", "/api/v3/simple/price?ids=mantle-staked-ether&vs_currencies=%CURRENCY%&include_24hr_change=true", "METH"},
  {"NOTCOIN", "/api/v3/simple/price?ids=notcoin&vs_currencies=%CURRENCY%&include_24hr_change=true", "NOT"},
  {"JUPITER", "/api/v3/simple/price?ids=jupiter-exchange-solana&vs_currencies=%CURRENCY%&include_24hr_change=true", "JUP"},
  {"THETA NETWORK", "/api/v3/simple/price?ids=theta-token&vs_currencies=%CURRENCY%&include_24hr_change=true", "THETA"},
  {"JASMYCOIN", "/api/v3/simple/price?ids=jasmycoin&vs_currencies=%CURRENCY%&include_24hr_change=true", "JASMY"},
  {"THORCHAIN", "/api/v3/simple/price?ids=thorchain&vs_currencies=%CURRENCY%&include_24hr_change=true", "RUNE"},
  {"ONDO", "/api/v3/simple/price?ids=ondo-finance&vs_currencies=%CURRENCY%&include_24hr_change=true", "ONDO"},
  {"PYTH NETWORK", "/api/v3/simple/price?ids=pyth-network&vs_currencies=%CURRENCY%&include_24hr_change=true", "PYTH"},
  {"CELESTIA", "/api/v3/simple/price?ids=celestia&vs_currencies=%CURRENCY%&include_24hr_change=true", "TIA"},
  {"LIDO DAO", "/api/v3/simple/price?ids=lido-dao&vs_currencies=%CURRENCY%&include_24hr_change=true", "LDO"},
  {"ALGORAND", "/api/v3/simple/price?ids=algorand&vs_currencies=%CURRENCY%&include_24hr_change=true", "ALGO"},
  {"CORE", "/api/v3/simple/price?ids=coredaoorg&vs_currencies=%CURRENCY%&include_24hr_change=true", "CORE"},
  {"BRETT", "/api/v3/simple/price?ids=based-brett&vs_currencies=%CURRENCY%&include_24hr_change=true", "BRETT"},
  {"QUANT", "/api/v3/simple/price?ids=quant-network&vs_currencies=%CURRENCY%&include_24hr_change=true", "QNT"},
  {"GATE", "/api/v3/simple/price?ids=gatechain-token&vs_currencies=%CURRENCY%&include_24hr_change=true", "GT"},
  {"FANTOM", "/api/v3/simple/price?ids=fantom&vs_currencies=%CURRENCY%&include_24hr_change=true", "FTM"},
  {"MANTRA", "/api/v3/simple/price?ids=mantra-dao&vs_currencies=%CURRENCY%&include_24hr_change=true", "OM"},
  {"ETHER.FI STAKED ETH", "/api/v3/simple/price?ids=ether-fi-staked-eth&vs_currencies=%CURRENCY%&include_24hr_change=true", "EETH"},
  {"FLOW", "/api/v3/simple/price?ids=flow&vs_currencies=%CURRENCY%&include_24hr_change=true", "FLOW"},
  {"SEI", "/api/v3/simple/price?ids=sei-network&vs_currencies=%CURRENCY%&include_24hr_change=true", "SEI"},
  {"HELIUM", "/api/v3/simple/price?ids=helium&vs_currencies=%CURRENCY%&include_24hr_change=true", "HNT"},
  {"MARINADE STAKED SOL", "/api/v3/simple/price?ids=msol&vs_currencies=%CURRENCY%&include_24hr_change=true", "MSOL"},
  {"BITCOIN SV", "/api/v3/simple/price?ids=bitcoin-cash-sv&vs_currencies=%CURRENCY%&include_24hr_change=true", "BSV"},
  {"FASTTOKEN", "/api/v3/simple/price?ids=fasttoken&vs_currencies=%CURRENCY%&include_24hr_change=true", "FTN"},
  {"USDD", "/api/v3/simple/price?ids=usdd&vs_currencies=%CURRENCY%&include_24hr_change=true", "USDD"},
  {"FLARE", "/api/v3/simple/price?ids=flare-networks&vs_currencies=%CURRENCY%&include_24hr_change=true", "FLR"},
  {"MULTIVERSX", "/api/v3/simple/price?ids=elrond-erd-2&vs_currencies=%CURRENCY%&include_24hr_change=true", "EGLD"},
  {"KUCOIN", "/api/v3/simple/price?ids=kucoin-shares&vs_currencies=%CURRENCY%&include_24hr_change=true", "KCS"},
  {"BITTORRENT", "/api/v3/simple/price?ids=bittorrent&vs_currencies=%CURRENCY%&include_24hr_change=true", "BTT"},
  {"EOS", "/api/v3/simple/price?ids=eos&vs_currencies=%CURRENCY%&include_24hr_change=true", "EOS"},
  {"PAYPAL USD", "/api/v3/simple/price?ids=paypal-usd&vs_currencies=%CURRENCY%&include_24hr_change=true", "PYUSD"},
  {"TEZOS", "/api/v3/simple/price?ids=tezos&vs_currencies=%CURRENCY%&include_24hr_change=true", "XTZ"},
  {"TOKENIZE XCHANGE", "/api/v3/simple/price?ids=tokenize-xchange&vs_currencies=%CURRENCY%&include_24hr_change=true", "TKX"},
  {"AXIE INFINITY", "/api/v3/simple/price?ids=axie-infinity&vs_currencies=%CURRENCY%&include_24hr_change=true", "AXS"},
  {"FRAX", "/api/v3/simple/price?ids=frax&vs_currencies=%CURRENCY%&include_24hr_change=true", "FRAX"},
  {"NEO", "/api/v3/simple/price?ids=neo&vs_currencies=%CURRENCY%&include_24hr_change=true", "NEO"},
  {"GALA", "/api/v3/simple/price?ids=gala&vs_currencies=%CURRENCY%&include_24hr_change=true", "GALA"},
  {"BEAM", "/api/v3/simple/price?ids=beam-2&vs_currencies=%CURRENCY%&include_24hr_change=true", "BEAM"},
  {"KELP DAO RESTAKED ETH", "/api/v3/simple/price?ids=kelp-dao-restaked-eth&vs_currencies=%CURRENCY%&include_24hr_change=true", "RSETH"},
  {"TETHER GOLD", "/api/v3/simple/price?ids=tether-gold&vs_currencies=%CURRENCY%&include_24hr_change=true", "XAUT"},
  {"ECASH", "/api/v3/simple/price?ids=ecash&vs_currencies=%CURRENCY%&include_24hr_change=true", "XEC"},
  {"AKASH NETWORK", "/api/v3/simple/price?ids=akash-network&vs_currencies=%CURRENCY%&include_24hr_change=true", "AKT"},
  {"STARKNET", "/api/v3/simple/price?ids=starknet&vs_currencies=%CURRENCY%&include_24hr_change=true", "STRK"},
  {"DYDX", "/api/v3/simple/price?ids=dydx-chain&vs_currencies=%CURRENCY%&include_24hr_change=true", "DYDX"},
  {"ETHEREUM NAME SERVICE", "/api/v3/simple/price?ids=ethereum-name-service&vs_currencies=%CURRENCY%&include_24hr_change=true", "ENS"},
  {"NEXO", "/api/v3/simple/price?ids=nexo&vs_currencies=%CURRENCY%&include_24hr_change=true", "NEXO"},
  {"THE SANDBOX", "/api/v3/simple/price?ids=the-sandbox&vs_currencies=%CURRENCY%&include_24hr_change=true", "SAND"},
  {"ORDI", "/api/v3/simple/price?ids=ordinals&vs_currencies=%CURRENCY%&include_24hr_change=true", "ORDI"},
  {"AIOZ NETWORK", "/api/v3/simple/price?ids=aioz-network&vs_currencies=%CURRENCY%&include_24hr_change=true", "AIOZ"},
  {"POPCAT", "/api/v3/simple/price?ids=popcat&vs_currencies=%CURRENCY%&include_24hr_change=true", "POPCAT"},
  {"WORLDCOIN", "/api/v3/simple/price?ids=worldcoin-wld&vs_currencies=%CURRENCY%&include_24hr_change=true", "WLD"},
  {"CONFLUX", "/api/v3/simple/price?ids=conflux-token&vs_currencies=%CURRENCY%&include_24hr_change=true", "CFX"},
  {"COINBASE WRAPPED STAKED ETH", "/api/v3/simple/price?ids=coinbase-wrapped-staked-eth&vs_currencies=%CURRENCY%&include_24hr_change=true", "CBETH"},
  {"CAT IN A DOGS WORLD", "/api/v3/simple/price?ids=cat-in-a-dogs-world&vs_currencies=%CURRENCY%&include_24hr_change=true", "MEW"},
  {"TRUEUSD", "/api/v3/simple/price?ids=true-usd&vs_currencies=%CURRENCY%&include_24hr_change=true", "TUSD"},
  {"WORMHOLE", "/api/v3/simple/price?ids=wormhole&vs_currencies=%CURRENCY%&include_24hr_change=true", "W"},
  {"ETHENA", "/api/v3/simple/price?ids=ethena&vs_currencies=%CURRENCY%&include_24hr_change=true", "ENA"},
  {"RONIN", "/api/v3/simple/price?ids=ronin&vs_currencies=%CURRENCY%&include_24hr_change=true", "RON"},
  {"DECENTRALAND", "/api/v3/simple/price?ids=decentraland&vs_currencies=%CURRENCY%&include_24hr_change=true", "MANA"},
  {"ZCASH", "/api/v3/simple/price?ids=zcash&vs_currencies=%CURRENCY%&include_24hr_change=true", "ZEC"},
  {"BOOK OF MEME", "/api/v3/simple/price?ids=book-of-meme&vs_currencies=%CURRENCY%&include_24hr_change=true", "BOME"},
  {"CHILIZ", "/api/v3/simple/price?ids=chiliz&vs_currencies=%CURRENCY%&include_24hr_change=true", "CHZ"},
  {"KLAYTN", "/api/v3/simple/price?ids=klay-token&vs_currencies=%CURRENCY%&include_24hr_change=true", "KLAY"},
  {"FRAX ETHER", "/api/v3/simple/price?ids=frax-ether&vs_currencies=%CURRENCY%&include_24hr_change=true", "FRXETH"},
  {"SATS (ORDINALS)", "/api/v3/simple/price?ids=sats-ordinals&vs_currencies=%CURRENCY%&include_24hr_change=true", "SATS"},
  {"MOG COIN", "/api/v3/simple/price?ids=mog-coin&vs_currencies=%CURRENCY%&include_24hr_change=true", "MOG"},
  {"PAX GOLD", "/api/v3/simple/price?ids=pax-gold&vs_currencies=%CURRENCY%&include_24hr_change=true", "PAXG"},
  {"MINA PROTOCOL", "/api/v3/simple/price?ids=mina-protocol&vs_currencies=%CURRENCY%&include_24hr_change=true", "MINA"},
  {"RAYDIUM", "/api/v3/simple/price?ids=raydium&vs_currencies=%CURRENCY%&include_24hr_change=true", "RAY"},
  {"IOTA", "/api/v3/simple/price?ids=iota&vs_currencies=%CURRENCY%&include_24hr_change=true", "IOTA"},
  {"SYNTHETIX NETWORK", "/api/v3/simple/price?ids=havven&vs_currencies=%CURRENCY%&include_24hr_change=true", "SNX"},
  {"USDB", "/api/v3/simple/price?ids=usdb&vs_currencies=%CURRENCY%&include_24hr_change=true", "USDB"},
  {"DEXE", "/api/v3/simple/price?ids=dexe&vs_currencies=%CURRENCY%&include_24hr_change=true", "DEXE"},
  {"PENDLE", "/api/v3/simple/price?ids=pendle&vs_currencies=%CURRENCY%&include_24hr_change=true", "PENDLE"},
  {"OASIS NETWORK", "/api/v3/simple/price?ids=oasis-network&vs_currencies=%CURRENCY%&include_24hr_change=true", "ROSE"},
  {"GNOSIS", "/api/v3/simple/price?ids=gnosis&vs_currencies=%CURRENCY%&include_24hr_change=true", "GNO"},
  {"SWELL ETHEREUM", "/api/v3/simple/price?ids=sweth&vs_currencies=%CURRENCY%&include_24hr_change=true", "SWETH"},
  {"APECOIN", "/api/v3/simple/price?ids=apecoin&vs_currencies=%CURRENCY%&include_24hr_change=true", "APE"},
  {"BITCOIN GOLD", "/api/v3/simple/price?ids=bitcoin-gold&vs_currencies=%CURRENCY%&include_24hr_change=true", "BTG"},
  {"ASTAR", "/api/v3/simple/price?ids=astar&vs_currencies=%CURRENCY%&include_24hr_change=true", "ASTR"},
  {"SAFEPAL", "/api/v3/simple/price?ids=safepal&vs_currencies=%CURRENCY%&include_24hr_change=true", "SFP"},
  {"XDC NETWORK", "/api/v3/simple/price?ids=xdce-crowd-sale&vs_currencies=%CURRENCY%&include_24hr_change=true", "XDC"},
  {"AXELAR", "/api/v3/simple/price?ids=axelar&vs_currencies=%CURRENCY%&include_24hr_change=true", "AXL"},
  {"AERODROME FINANCE", "/api/v3/simple/price?ids=aerodrome-finance&vs_currencies=%CURRENCY%&include_24hr_change=true", "AERO"},
  {"APENFT", "/api/v3/simple/price?ids=apenft&vs_currencies=%CURRENCY%&include_24hr_change=true", "NFT"},
  {"ZKSYNC", "/api/v3/simple/price?ids=zksync&vs_currencies=%CURRENCY%&include_24hr_change=true", "ZK"},
  {"TERRA LUNA CLASSIC", "/api/v3/simple/price?ids=terra-luna&vs_currencies=%CURRENCY%&include_24hr_change=true", "LUNC"},
  {"STAKED FRAX ETHER", "/api/v3/simple/price?ids=staked-frax-ether&vs_currencies=%CURRENCY%&include_24hr_change=true", "SFRXETH"},
  {"THETA FUEL", "/api/v3/simple/price?ids=theta-fuel&vs_currencies=%CURRENCY%&include_24hr_change=true", "TFUEL"},
  {"LAYERZERO", "/api/v3/simple/price?ids=layerzero&vs_currencies=%CURRENCY%&include_24hr_change=true", "ZRO"},
  {"PANCAKESWAP", "/api/v3/simple/price?ids=pancakeswap-token&vs_currencies=%CURRENCY%&include_24hr_change=true", "CAKE"},
  {"TRUST WALLET", "/api/v3/simple/price?ids=trust-wallet-token&vs_currencies=%CURRENCY%&include_24hr_change=true", "TWT"},
  {"LIVEPEER", "/api/v3/simple/price?ids=livepeer&vs_currencies=%CURRENCY%&include_24hr_change=true", "LPT"},
  {"NERVOS NETWORK", "/api/v3/simple/price?ids=nervos-network&vs_currencies=%CURRENCY%&include_24hr_change=true", "CKB"},
  {"BELDEX", "/api/v3/simple/price?ids=beldex&vs_currencies=%CURRENCY%&include_24hr_change=true", "BDX"},
  {"H2O DAO", "/api/v3/simple/price?ids=h2o-dao&vs_currencies=%CURRENCY%&include_24hr_change=true", "H2O"},
  {"ONDO US DOLLAR YIELD", "/api/v3/simple/price?ids=ondo-us-dollar-yield&vs_currencies=%CURRENCY%&include_24hr_change=true", "USDY"},
  {"MX", "/api/v3/simple/price?ids=mx-token&vs_currencies=%CURRENCY%&include_24hr_change=true", "MX"},
  {"1INCH", "/api/v3/simple/price?ids=1inch&vs_currencies=%CURRENCY%&include_24hr_change=true", "1INCH"},
  {"KAVA", "/api/v3/simple/price?ids=kava&vs_currencies=%CURRENCY%&include_24hr_change=true", "KAVA"},
  {"COMPOUND", "/api/v3/simple/price?ids=compound-governance-token&vs_currencies=%CURRENCY%&include_24hr_change=true", "COMP"},
  {"SAFE", "/api/v3/simple/price?ids=safe&vs_currencies=%CURRENCY%&include_24hr_change=true", "SAFE"},
  {"IOTEX", "/api/v3/simple/price?ids=iotex&vs_currencies=%CURRENCY%&include_24hr_change=true", "IOTX"},
  {"ARAGON", "/api/v3/simple/price?ids=aragon&vs_currencies=%CURRENCY%&include_24hr_change=true", "ANT"},
  {"WEMIX", "/api/v3/simple/price?ids=wemix-token&vs_currencies=%CURRENCY%&include_24hr_change=true", "WEMIX"},
  {"JITO", "/api/v3/simple/price?ids=jito-governance-token&vs_currencies=%CURRENCY%&include_24hr_change=true", "JTO"},
  {"BITCOIN AVALANCHE BRIDGED (BTC.B)", "/api/v3/simple/price?ids=bitcoin-avalanche-bridged-btc-b&vs_currencies=%CURRENCY%&include_24hr_change=true", "BTC.B"},
  {"DOG•GO•TO•THE•MOON (RUNES)", "/api/v3/simple/price?ids=dog-go-to-the-moon-rune&vs_currencies=%CURRENCY%&include_24hr_change=true", "DOG"},
  {"GALXE", "/api/v3/simple/price?ids=project-galaxy&vs_currencies=%CURRENCY%&include_24hr_change=true", "GAL"},
  {"TRIBE", "/api/v3/simple/price?ids=tribe-2&vs_currencies=%CURRENCY%&include_24hr_change=true", "TRIBE"},
  {"AEVO", "/api/v3/simple/price?ids=aevo-exchange&vs_currencies=%CURRENCY%&include_24hr_change=true", "AEVO"},
  {"NEIRO ON ETH", "/api/v3/simple/price?ids=neiro-on-eth&vs_currencies=%CURRENCY%&include_24hr_change=true", "NEIRO"},
  {"GRAVITY", "/api/v3/simple/price?ids=g-token&vs_currencies=%CURRENCY%&include_24hr_change=true", "G"},
  {"TURBO", "/api/v3/simple/price?ids=turbo&vs_currencies=%CURRENCY%&include_24hr_change=true", "TURBO"},
  {"STADER ETHX", "/api/v3/simple/price?ids=stader-ethx&vs_currencies=%CURRENCY%&include_24hr_change=true", "ETHX"},
  {"CONSTITUTIONDAO", "/api/v3/simple/price?ids=constitutiondao&vs_currencies=%CURRENCY%&include_24hr_change=true", "PEOPLE"},
  {"CORGIAI", "/api/v3/simple/price?ids=corgiai&vs_currencies=%CURRENCY%&include_24hr_change=true", "CORGIAI"},
  {"ILLUVIUM", "/api/v3/simple/price?ids=illuvium&vs_currencies=%CURRENCY%&include_24hr_change=true", "ILV"},
  {"CURVE DAO", "/api/v3/simple/price?ids=curve-dao-token&vs_currencies=%CURRENCY%&include_24hr_change=true", "CRV"},
  {"VENOM", "/api/v3/simple/price?ids=venom&vs_currencies=%CURRENCY%&include_24hr_change=true", "VENOM"},
  {"GOLEM", "/api/v3/simple/price?ids=golem&vs_currencies=%CURRENCY%&include_24hr_change=true", "GLM"},
  {"DASH", "/api/v3/simple/price?ids=dash&vs_currencies=%CURRENCY%&include_24hr_change=true", "DASH"},
  {"RADIX", "/api/v3/simple/price?ids=radix&vs_currencies=%CURRENCY%&include_24hr_change=true", "XRD"},
  {"WOO", "/api/v3/simple/price?ids=woo-network&vs_currencies=%CURRENCY%&include_24hr_change=true", "WOO"},
  {"AELF", "/api/v3/simple/price?ids=aelf&vs_currencies=%CURRENCY%&include_24hr_change=true", "ELF"},
  {"JUST", "/api/v3/simple/price?ids=just&vs_currencies=%CURRENCY%&include_24hr_change=true", "JST"},
  {"ECHELON PRIME", "/api/v3/simple/price?ids=echelon-prime&vs_currencies=%CURRENCY%&include_24hr_change=true", "PRIME"},
  {"MEMECOIN", "/api/v3/simple/price?ids=memecoin-2&vs_currencies=%CURRENCY%&include_24hr_change=true", "MEME"},
  {"ARBITRUM BRIDGED USDC (ARBITRUM)", "/api/v3/simple/price?ids=usd-coin-ethereum-bridged&vs_currencies=%CURRENCY%&include_24hr_change=true", "USDC.E"},
  {"KUSAMA", "/api/v3/simple/price?ids=kusama&vs_currencies=%CURRENCY%&include_24hr_change=true", "KSM"},
  {"MANTA NETWORK", "/api/v3/simple/price?ids=manta-network&vs_currencies=%CURRENCY%&include_24hr_change=true", "MANTA"},
  {"CWBTC", "/api/v3/simple/price?ids=compound-wrapped-btc&vs_currencies=%CURRENCY%&include_24hr_change=true", "CWBTC"},
  {"BLUR", "/api/v3/simple/price?ids=blur&vs_currencies=%CURRENCY%&include_24hr_change=true", "BLUR"},
  {"OSMOSIS", "/api/v3/simple/price?ids=osmosis&vs_currencies=%CURRENCY%&include_24hr_change=true", "OSMO"},
  {"ZILLIQA", "/api/v3/simple/price?ids=zilliqa&vs_currencies=%CURRENCY%&include_24hr_change=true", "ZIL"},
  {"SIACOIN", "/api/v3/simple/price?ids=siacoin&vs_currencies=%CURRENCY%&include_24hr_change=true", "SC"},
  {"AETHIR", "/api/v3/simple/price?ids=aethir&vs_currencies=%CURRENCY%&include_24hr_change=true", "ATH"},
  {"ARKHAM", "/api/v3/simple/price?ids=arkham&vs_currencies=%CURRENCY%&include_24hr_change=true", "ARKM"},
  {"ANKR NETWORK", "/api/v3/simple/price?ids=ankr&vs_currencies=%CURRENCY%&include_24hr_change=true", "ANKR"},
  {"ROCKET POOL", "/api/v3/simple/price?ids=rocket-pool&vs_currencies=%CURRENCY%&include_24hr_change=true", "RPL"},
  {"0X PROTOCOL", "/api/v3/simple/price?ids=0x&vs_currencies=%CURRENCY%&include_24hr_change=true", "ZRX"},
  {"CELO", "/api/v3/simple/price?ids=celo&vs_currencies=%CURRENCY%&include_24hr_change=true", "CELO"},
  {"HOLO", "/api/v3/simple/price?ids=holotoken&vs_currencies=%CURRENCY%&include_24hr_change=true", "HOT"},
  {"GMT", "/api/v3/simple/price?ids=stepn&vs_currencies=%CURRENCY%&include_24hr_change=true", "GMT"},
  {"BASIC ATTENTION", "/api/v3/simple/price?ids=basic-attention-token&vs_currencies=%CURRENCY%&include_24hr_change=true", "BAT"},
  {"DYMENSION", "/api/v3/simple/price?ids=dymension&vs_currencies=%CURRENCY%&include_24hr_change=true", "DYM"},
  {"SINGULARITYNET", "/api/v3/simple/price?ids=singularitynet&vs_currencies=%CURRENCY%&include_24hr_change=true", "AGIX"},
  {"OUSG", "/api/v3/simple/price?ids=ousg&vs_currencies=%CURRENCY%&include_24hr_change=true", "OUSG"},
  {"ETHER.FI", "/api/v3/simple/price?ids=ether-fi&vs_currencies=%CURRENCY%&include_24hr_change=true", "ETHFI"},
  {"PEPECOIN", "/api/v3/simple/price?ids=pepecoin-2&vs_currencies=%CURRENCY%&include_24hr_change=true", "PEPECOIN"},
  {"QTUM", "/api/v3/simple/price?ids=qtum&vs_currencies=%CURRENCY%&include_24hr_change=true", "QTUM"},
  {"ENJIN COIN", "/api/v3/simple/price?ids=enjincoin&vs_currencies=%CURRENCY%&include_24hr_change=true", "ENJ"},
  {"LIQUID STAKED ETH", "/api/v3/simple/price?ids=liquid-staked-ethereum&vs_currencies=%CURRENCY%&include_24hr_change=true", "LSETH"},
  {"DYDX", "/api/v3/simple/price?ids=dydx&vs_currencies=%CURRENCY%&include_24hr_change=true", "ETHDYDX"},
  {"TERRA", "/api/v3/simple/price?ids=terra-luna-2&vs_currencies=%CURRENCY%&include_24hr_change=true", "LUNA"},
  {"RAVENCOIN", "/api/v3/simple/price?ids=ravencoin&vs_currencies=%CURRENCY%&include_24hr_change=true", "RVN"},
  {"OLYMPUS", "/api/v3/simple/price?ids=olympus&vs_currencies=%CURRENCY%&include_24hr_change=true", "OHM"},
  {"AMP", "/api/v3/simple/price?ids=amp-token&vs_currencies=%CURRENCY%&include_24hr_change=true", "AMP"},
  {"MMX", "/api/v3/simple/price?ids=m2-global-wealth-limited-mmx&vs_currencies=%CURRENCY%&include_24hr_change=true", "MMX"},
  {"POLYMESH", "/api/v3/simple/price?ids=polymesh&vs_currencies=%CURRENCY%&include_24hr_change=true", "POLYX"},
  {"SUPERVERSE", "/api/v3/simple/price?ids=superfarm&vs_currencies=%CURRENCY%&include_24hr_change=true", "SUPER"},
  {"ORIGINTRAIL", "/api/v3/simple/price?ids=origintrail&vs_currencies=%CURRENCY%&include_24hr_change=true", "TRAC"},
  {"GMX", "/api/v3/simple/price?ids=gmx&vs_currencies=%CURRENCY%&include_24hr_change=true", "GMX"},
  {"ETHEREUMPOW", "/api/v3/simple/price?ids=ethereum-pow-iou&vs_currencies=%CURRENCY%&include_24hr_change=true", "ETHW"},
  {"NEM", "/api/v3/simple/price?ids=nem&vs_currencies=%CURRENCY%&include_24hr_change=true", "XEM"},
  {"GAS", "/api/v3/simple/price?ids=gas&vs_currencies=%CURRENCY%&include_24hr_change=true", "GAS"},
  {"ALTLAYER", "/api/v3/simple/price?ids=altlayer&vs_currencies=%CURRENCY%&include_24hr_change=true", "ALT"},
  {"MIMBLEWIMBLECOIN", "/api/v3/simple/price?ids=mimblewimblecoin&vs_currencies=%CURRENCY%&include_24hr_change=true", "MWC"},
  {"ULTIMA", "/api/v3/simple/price?ids=ultima&vs_currencies=%CURRENCY%&include_24hr_change=true", "ULTIMA"},
  {"BICONOMY", "/api/v3/simple/price?ids=biconomy&vs_currencies=%CURRENCY%&include_24hr_change=true", "BICO"},
  {"NEXUS MUTUAL", "/api/v3/simple/price?ids=nxm&vs_currencies=%CURRENCY%&include_24hr_change=true", "NXM"},
  {"MASK NETWORK", "/api/v3/simple/price?ids=mask-network&vs_currencies=%CURRENCY%&include_24hr_change=true", "MASK"},
  {"CONVEX FINANCE", "/api/v3/simple/price?ids=convex-finance&vs_currencies=%CURRENCY%&include_24hr_change=true", "CVX"},
  {"COINEX", "/api/v3/simple/price?ids=coinex-token&vs_currencies=%CURRENCY%&include_24hr_change=true", "CET"},
  {"WRAPPED BEACON ETH", "/api/v3/simple/price?ids=wrapped-beacon-eth&vs_currencies=%CURRENCY%&include_24hr_change=true", "WBETH"},
  {"RESERVE RIGHTS", "/api/v3/simple/price?ids=reserve-rights-token&vs_currencies=%CURRENCY%&include_24hr_change=true", "RSR"},
  {"CETH", "/api/v3/simple/price?ids=compound-ether&vs_currencies=%CURRENCY%&include_24hr_change=true", "CETH"},
  {"PONKE", "/api/v3/simple/price?ids=ponke&vs_currencies=%CURRENCY%&include_24hr_change=true", "PONKE"},
  {"THRESHOLD NETWORK", "/api/v3/simple/price?ids=threshold-network-token&vs_currencies=%CURRENCY%&include_24hr_change=true", "T"},
  {"METIS", "/api/v3/simple/price?ids=metis-token&vs_currencies=%CURRENCY%&include_24hr_change=true", "METIS"},
  {"QUBIC", "/api/v3/simple/price?ids=qubic-network&vs_currencies=%CURRENCY%&include_24hr_change=true", "QUBIC"},
  {"TBTC", "/api/v3/simple/price?ids=tbtc&vs_currencies=%CURRENCY%&include_24hr_change=true", "TBTC"},
  {"CASPER NETWORK", "/api/v3/simple/price?ids=casper-network&vs_currencies=%CURRENCY%&include_24hr_change=true", "CSPR"},
  {"BLAST", "/api/v3/simple/price?ids=blast&vs_currencies=%CURRENCY%&include_24hr_change=true", "BLAST"},
  {"SKALE", "/api/v3/simple/price?ids=skale&vs_currencies=%CURRENCY%&include_24hr_change=true", "SKL"},
  {"PAAL AI", "/api/v3/simple/price?ids=paal-ai&vs_currencies=%CURRENCY%&include_24hr_change=true", "PAAL"},
  {"AVAIL", "/api/v3/simple/price?ids=avail&vs_currencies=%CURRENCY%&include_24hr_change=true", "AVAIL"},
  {"BENQI LIQUID STAKED AVAX", "/api/v3/simple/price?ids=benqi-liquid-staked-avax&vs_currencies=%CURRENCY%&include_24hr_change=true", "SAVAX"},
  {"GOLDFINCH", "/api/v3/simple/price?ids=goldfinch&vs_currencies=%CURRENCY%&include_24hr_change=true", "GFI"},
  {"HIVEMAPPER", "/api/v3/simple/price?ids=hivemapper&vs_currencies=%CURRENCY%&include_24hr_change=true", "HONEY"},
  {"CENTRIFUGE", "/api/v3/simple/price?ids=centrifuge&vs_currencies=%CURRENCY%&include_24hr_change=true", "CFG"},
  {"FLUX", "/api/v3/simple/price?ids=zelcash&vs_currencies=%CURRENCY%&include_24hr_change=true", "FLUX"},
  {"ALCHEMIX USD", "/api/v3/simple/price?ids=alchemix-usd&vs_currencies=%CURRENCY%&include_24hr_change=true", "ALUSD"},
  {"CHIA", "/api/v3/simple/price?ids=chia&vs_currencies=%CURRENCY%&include_24hr_change=true", "XCH"},
  {"OSAKA PROTOCOL", "/api/v3/simple/price?ids=osaka-protocol&vs_currencies=%CURRENCY%&include_24hr_change=true", "OSAK"},
  {"NON-PLAYABLE COIN", "/api/v3/simple/price?ids=non-playable-coin&vs_currencies=%CURRENCY%&include_24hr_change=true", "NPC"},
  {"SWISSBORG", "/api/v3/simple/price?ids=swissborg&vs_currencies=%CURRENCY%&include_24hr_change=true", "BORG"},
  {"DECRED", "/api/v3/simple/price?ids=decred&vs_currencies=%CURRENCY%&include_24hr_change=true", "DCR"},
  {"YEARN.FINANCE", "/api/v3/simple/price?ids=yearn-finance&vs_currencies=%CURRENCY%&include_24hr_change=true", "YFI"},
  {"IO.NET", "/api/v3/simple/price?ids=io&vs_currencies=%CURRENCY%&include_24hr_change=true", "IO"},
  {"ARCBLOCK", "/api/v3/simple/price?ids=arcblock&vs_currencies=%CURRENCY%&include_24hr_change=true", "ABT"},
  {"UMA", "/api/v3/simple/price?ids=uma&vs_currencies=%CURRENCY%&include_24hr_change=true", "UMA"},
  {"VETHOR", "/api/v3/simple/price?ids=vethor-token&vs_currencies=%CURRENCY%&include_24hr_change=true", "VTHO"},
  {"MUMU THE BULL", "/api/v3/simple/price?ids=mumu-the-bull-3&vs_currencies=%CURRENCY%&include_24hr_change=true", "MUMU"},
  {"MAGA", "/api/v3/simple/price?ids=maga&vs_currencies=%CURRENCY%&include_24hr_change=true", "TRUMP"},
  {"API3", "/api/v3/simple/price?ids=api3&vs_currencies=%CURRENCY%&include_24hr_change=true", "API3"},
  {"BABY DOGE COIN", "/api/v3/simple/price?ids=baby-doge-coin&vs_currencies=%CURRENCY%&include_24hr_change=true", "BABYDOGE"},
  {"LOOPRING", "/api/v3/simple/price?ids=loopring&vs_currencies=%CURRENCY%&include_24hr_change=true", "LRC"},
  {"BAND PROTOCOL", "/api/v3/simple/price?ids=band-protocol&vs_currencies=%CURRENCY%&include_24hr_change=true", "BAND"},
  {"MYTHOS", "/api/v3/simple/price?ids=mythos&vs_currencies=%CURRENCY%&include_24hr_change=true", "MYTH"},
  {"SPACE ID", "/api/v3/simple/price?ids=space-id&vs_currencies=%CURRENCY%&include_24hr_change=true", "ID"},
  {"ZIGNALY", "/api/v3/simple/price?ids=zignaly&vs_currencies=%CURRENCY%&include_24hr_change=true", "ZIG"},
  {"FRAX SHARE", "/api/v3/simple/price?ids=frax-share&vs_currencies=%CURRENCY%&include_24hr_change=true", "FXS"},
  {"GIGACHAD", "/api/v3/simple/price?ids=gigachad-2&vs_currencies=%CURRENCY%&include_24hr_change=true", "GIGA"},
  {"TELLOR TRIBUTES", "/api/v3/simple/price?ids=tellor&vs_currencies=%CURRENCY%&include_24hr_change=true", "TRB"},
  {"ALEPH ZERO", "/api/v3/simple/price?ids=aleph-zero&vs_currencies=%CURRENCY%&include_24hr_change=true", "AZERO"},
  {"ZETACHAIN", "/api/v3/simple/price?ids=zetachain&vs_currencies=%CURRENCY%&include_24hr_change=true", "ZETA"},
  {"CREDITCOIN", "/api/v3/simple/price?ids=creditcoin-2&vs_currencies=%CURRENCY%&include_24hr_change=true", "CTC"},
  {"ONTOLOGY", "/api/v3/simple/price?ids=ontology&vs_currencies=%CURRENCY%&include_24hr_change=true", "ONT"},
  {"AUDIUS", "/api/v3/simple/price?ids=audius&vs_currencies=%CURRENCY%&include_24hr_change=true", "AUDIO"},
  {"MOONBEAM", "/api/v3/simple/price?ids=moonbeam&vs_currencies=%CURRENCY%&include_24hr_change=true", "GLMR"},
  {"METAPLEX", "/api/v3/simple/price?ids=metaplex&vs_currencies=%CURRENCY%&include_24hr_change=true", "MPLX"},
  {"STASIS EURO", "/api/v3/simple/price?ids=stasis-eurs&vs_currencies=%CURRENCY%&include_24hr_change=true", "EURS"},
  {"FINSCHIA", "/api/v3/simple/price?ids=link&vs_currencies=%CURRENCY%&include_24hr_change=true", "FNSA"},
  {"ALEPHIUM", "/api/v3/simple/price?ids=alephium&vs_currencies=%CURRENCY%&include_24hr_change=true", "ALPH"},
  {"OCEAN PROTOCOL", "/api/v3/simple/price?ids=ocean-protocol&vs_currencies=%CURRENCY%&include_24hr_change=true", "OCEAN"},
  {"SSV NETWORK", "/api/v3/simple/price?ids=ssv-network&vs_currencies=%CURRENCY%&include_24hr_change=true", "SSV"},
  {"ESCOIN", "/api/v3/simple/price?ids=escoin-token&vs_currencies=%CURRENCY%&include_24hr_change=true", "ELG"},
  {"CHROMIA", "/api/v3/simple/price?ids=chromaway&vs_currencies=%CURRENCY%&include_24hr_change=true", "CHR"},
  {"ROLLBIT COIN", "/api/v3/simple/price?ids=rollbit-coin&vs_currencies=%CURRENCY%&include_24hr_change=true", "RLB"},
  {"HARMONY", "/api/v3/simple/price?ids=harmony&vs_currencies=%CURRENCY%&include_24hr_change=true", "ONE"},
  {"ICON", "/api/v3/simple/price?ids=icon&vs_currencies=%CURRENCY%&include_24hr_change=true", "ICX"},
  {"ACROSS PROTOCOL", "/api/v3/simple/price?ids=across-protocol&vs_currencies=%CURRENCY%&include_24hr_change=true", "ACX"},
  {"NOSANA", "/api/v3/simple/price?ids=nosana&vs_currencies=%CURRENCY%&include_24hr_change=true", "NOS"},
  {"BLOX", "/api/v3/simple/price?ids=blox&vs_currencies=%CURRENCY%&include_24hr_change=true", "CDT"},
  {"KADENA", "/api/v3/simple/price?ids=kadena&vs_currencies=%CURRENCY%&include_24hr_change=true", "KDA"},
  {"COTI", "/api/v3/simple/price?ids=coti&vs_currencies=%CURRENCY%&include_24hr_change=true", "COTI"},
  {"GOMINING TOKEN", "/api/v3/simple/price?ids=gmt-token&vs_currencies=%CURRENCY%&include_24hr_change=true", "GOMINING"},
  {"TELCOIN", "/api/v3/simple/price?ids=telcoin&vs_currencies=%CURRENCY%&include_24hr_change=true", "TEL"}
};

// Корневая страница HTTP
void handleRoot() {
  String html = "<!DOCTYPE html><html><body>";
  html += "<h2>Enter Wi-Fi Credentials and ID</h2>";
  html += "<form action=\"/submit\" method=\"POST\">";
  html += "Wi-Fi SSID:<br><input type=\"text\" name=\"ssid\"><br>";
  html += "Password:<br><input type=\"text\" name=\"password\"><br>";
  html += "ID:<br><input type=\"text\" name=\"id\"><br><br>";
  html += "<input type=\"submit\" value=\"Submit\">";
  html += "</form></body></html>";

  server.send(200, "text/html", html);
}

// Функция для обработки данных, отправленных через форму
void handleSubmit() {
  if (server.hasArg("ssid") && server.hasArg("password") && server.hasArg("id")) {
    wifiSSID = server.arg("ssid");
    wifiPassword = server.arg("password");
    userID = server.arg("id");

    Serial.println("Received Data:");
    Serial.print("SSID: ");
    Serial.println(wifiSSID);
    Serial.print("Password: ");
    Serial.println(wifiPassword);
    Serial.print("ID: ");
    Serial.println(userID);
    CHAT_ID = userID;

    String response = "<!DOCTYPE html><html><body><h2>Data Submitted Successfully</h2>";
    response += "<p>SSID: " + wifiSSID + "</p>";
    response += "<p>Password: " + wifiPassword + "</p>";
    response += "<p>ID: " + userID + "</p>";
    response += "<br><br><a href=\"/\">Go Back</a>";
    response += "</body></html>";
    server.send(200, "text/html", response);

    // Отключаем точку доступа и пробуем подключиться к введенной сети Wi-Fi
    WiFi.softAPdisconnect(true);
    delay(1000);
    WiFi.begin(wifiSSID.c_str(), wifiPassword.c_str());

    display.clearDisplay();
    display.setTextSize(2);
    printCenter("Connecting", 0, 25);
    display.setTextSize(1);
    printCenter("Please wait", 0, 47);
    display.display();

    int attempts = 0;
    while (WiFi.status() != WL_CONNECTED && attempts < 10) {
      delay(1000);
      Serial.print(".");
      attempts++;
    }

    if (WiFi.status() == WL_CONNECTED) {
      display.clearDisplay();
      display.setTextSize(2);
      printCenter("Done", 0, 20);
      display.setTextSize(1);
      printCenter("Telegram bot:", 0, 40);
      display.setTextSize(1);
      printCenter("CryptoTimeParser_bot", 0, 52);
      display.display();

      Serial.println("");
      Serial.println("WiFi connected.");
      Serial.println("IP address: ");
      Serial.println(WiFi.localIP());
    } 
    else {
      display.clearDisplay();
      display.setTextSize(1);
      printCenter("Connection error", 0, 27);
      display.setTextSize(1);
      printCenter("Wait 5 seconds...", 0, 38);
      display.display();

      delay(5000);

      Serial.println("");
      Serial.println("Failed to connect. Restarting Access Point...");
      startAccessPoint();
    }
  } 
  else {
    server.send(400, "text/plain", "400: Invalid Request");
  }
}

// Функция для включения точки доступа
void startAccessPoint() {
  WiFi.softAP("CryptoTime");
  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);

  display.clearDisplay();
  display.setTextSize(1);
  printCenter("Wi-Fi point name:", 0, 7);
  display.setTextSize(2);
  printCenter("CryptoTime", 0, 20);
  display.setTextSize(1);
  printCenter("IP address:", 0, 43);
  display.setTextSize(1);
  printCenter(IP.toString(), 0, 57);
  display.display();
}

// Функция для поиска элемента по имени монеты
void getCoinData(String coinName) {
  // Преобразуем имя монеты в верхний регистр
  coinName.toUpperCase();
  Serial.println(coinName);

  for (int i = 0; i < coin_num; i++) {
    // Сравниваем с именами монет в верхнем регистре
    if (coinDict[i].coinName == coinName || coinDict[i].abbreviation == coinName) {
      // Подстановка выбранной валюты
      URL = coinDict[i].url;
      
      Serial.print("URL: ");
      Serial.println(URL);

      Serial.print("Abbreviation: ");
      Serial.println(coinDict[i].abbreviation);
      
      Ticker = coinDict[i].abbreviation;
      CoinSearch = true;
      priceUpAlert = -1;
      priceDownAlert = -1;
      return;
    }
  }
  CoinSearch = false;
  Serial.println("Coin not found!");
}

// Функция для отправки запроса и получения ответа
String sendRequest(const char* url) {
  Serial.print("URL: ");
  Serial.println(url);
  WiFiClientSecure client; // Для выполнения HTTPS запросов
  client.setInsecure(); // Отключение проверки SSL сертификата

  if (!client.connect(host, httpsPort)) { // Установка соединения с сервером
    Serial.println("Connection failed."); 
    return "";
  }
  
  // Отправляет HTTP GET запрос на сервер
  client.print(String("GET ") + url + " HTTP/1.1\r\n" + // Метод запроса URL
               "Host: " + host + "\r\n" + // Указание хоста
               "Connection: close\r\n\r\n"); // Закрывать соединение после выполнения

  String payload = ""; // Запись ответа
  bool headersEnded = false; // Заголовк HTTP не завершен

  while (client.connected() || client.available()) { // Пока клиент подключен или имеются доступные данные
    if (client.available()) { // Есть ли данные, доступные для чтения
      String line = client.readStringUntil('\n'); // Читает строку данных до символа новой строки 
      if (line == "\r") { // Проверяет, является ли текущая строка пустой, что означает окончание заголовков HTTP
        headersEnded = true; // Заголовки завершены
        continue;
      }
      if (headersEnded) { // Если заголовки завершены, добавляет строку в переменную payload
        payload += line;
      }
    }
  }

  client.stop(); // Закрывает соединение с сервером
  return payload; // Возвращает собранный ответ от сервера
}

// Функция обработки ответа от сервера
void CoinData(const char* url) {
  Serial.print("Using CurrencyRate: ");
  Serial.println(CurrencyRate);

  String urlString = String(url); // Преобразование const char* в String
  urlString.replace("%CURRENCY%", CurrencyRate); // Замена %CURRENCY% на текущую валюту

  String payload = sendRequest(urlString.c_str()); // Отправка url для получения ответа от сервера

  // Вывод полного ответа для отладки
  Serial.print("Response for URL "); // Выводит сообщение о текущем URL
  Serial.println(url); // Выводит URL запроса
  Serial.println("Full response:"); // Выводит заголовок для полного ответа
  Serial.println(payload); // Выводит полный ответ от сервера для отладки
  
  // Удаление лишних данных из ответа
  int startIndex = payload.indexOf('{'); // Найти начало JSON
  int endIndex = payload.lastIndexOf('}'); // Найти конец JSON

  // Проверяет, что индексы корректны и не пересекаются.
  // Если индексы некорректны, выводит сообщение об ошибке и переходит к следующему URL
  if (startIndex != -1 && endIndex != -1 && startIndex < endIndex) {

    // Извлекает подстроку из полного ответа, содержащую только JSON данные
    String jsonPayload = payload.substring(startIndex, endIndex + 1);

    // Вывод очищенного JSON для отладки
    Serial.println("Cleaned JSON response:");
    Serial.println(jsonPayload); // Выводит очищенный JSON для отладки

    // Парсинг JSON-ответа
    StaticJsonDocument<256> doc; // Создаёт объект StaticJsonDocument с размером 256 байт для хранения данных JSON
    // Десериализует JSON из строки в объект doc. Возвращает ошибку, если десериализация не удалась
    DeserializationError error = deserializeJson(doc, jsonPayload);
    
    if (!error) { // Проверяет, произошла ли ошибка при десериализации
      if (!doc.isNull() || doc.as<JsonObject>().size()){ // Проверяет, что объект JSON не пуст и содержит данные
        // Извлечение значений из JSON
        for (JsonPair kv : doc.as<JsonObject>()) { // Проходит по каждому элементу JSON объекта
          const char* key = kv.key().c_str(); // Извлекает ключ из пары JSON
          float coin_change;
          if (CurrencyRate == "USD"){
            coin = kv.value()["usd"].as<float>(); // Извлекает значение usd из JSON объекта и преобразует его в float
            coin_change = kv.value()["usd_24h_change"].as<float>(); // Извлекает значение usd_24h_change из JSON объекта и преобразует его в float
          }
          else if (CurrencyRate == "RUB"){
            coin = kv.value()["rub"].as<float>(); // Извлекает значение usd из JSON объекта и преобразует его в float
            coin_change = kv.value()["rub_24h_change"].as<float>(); // Извлекает значение usd_24h_change из JSON объекта и преобразует его в float
          }
          else if (CurrencyRate == "EUR"){
            coin = kv.value()["eur"].as<float>(); // Извлекает значение usd из JSON объекта и преобразует его в float
            coin_change = kv.value()["eur_24h_change"].as<float>(); // Извлекает значение usd_24h_change из JSON объекта и преобразует его в float
          }
          else if (CurrencyRate == "GBP"){
            coin = kv.value()["gbp"].as<float>(); // Извлекает значение usd из JSON объекта и преобразует его в float
            coin_change = kv.value()["gbp_24h_change"].as<float>(); // Извлекает значение usd_24h_change из JSON объекта и преобразует его в float
          }
          else if (CurrencyRate == "CNY"){
            coin = kv.value()["cny"].as<float>(); // Извлекает значение usd из JSON объекта и преобразует его в float
            coin_change = kv.value()["cny_24h_change"].as<float>(); // Извлекает значение usd_24h_change из JSON объекта и преобразует его в float
          }
          else if (CurrencyRate == "JPY"){
            coin = kv.value()["jpy"].as<float>(); // Извлекает значение usd из JSON объекта и преобразует его в float
            coin_change = kv.value()["jpy_24h_change"].as<float>(); // Извлекает значение usd_24h_change из JSON объекта и преобразует его в float
          }
          else{
            Serial.println("ERROR");
          }
          if (coin == static_cast<int>(coin)) { // Проверка, является ли число целым
            Price_display = SymbolCoin + String(coin, 0);
          }
          else if(coin < 1){
            Price_display = SymbolCoin + String(coin, 6);
          }
          else{
            Price_display = SymbolCoin + String(coin);
          }
          Serial.print("Изменение: ");
          Serial.println(coin_change, 4);
          DayPrice_display = coin_change;
        }
      }
    }
    else{
      Serial.print("deserializeJson() failed: "); // Выводит сообщение об ошибке десериализации
      Serial.println(error.c_str()); // Текст ошибки
    }
  }
  else{
    Serial.println("Invalid JSON format."); // Выводит сообщение, если JSON пуст или невалиден
    Price_display = "Undefined";
    DayPrice_display = 0.0000;
  }
}

void setup() {
  Serial.begin(115200);

  Serial.print("Flash: ");
  Serial.println(ESP.getFlashChipSize());

  // Инициализация дисплея
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.setTextColor(WHITE);
  display.clearDisplay();
  display.display();
  
  /*
  Serial.println("Connecting to Wi-Fi");
  WiFi.begin("Your Wi-Fi SSID", "Your Wi-Fi password");
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  
  Serial.println("");
  Serial.println("WiFi connected.");
  */

  startAccessPoint();

  // Настройка маршрутов веб-сервера
  server.on("/", handleRoot); // Обработка корневого URL "/"
  server.on("/submit", HTTP_POST, handleSubmit); // Обработка отправки данных через форму

  server.begin();
  Serial.println("HTTP server started");

  // Установка SSL для Telegram
  client.setCACert(TELEGRAM_CERTIFICATE_ROOT);
}

// Функция обработки ответа пользователя
void handleNewMessages(int numNewMessages) {
  Serial.print("handleNewMessages: ");
  Serial.println(String(numNewMessages));

  for (int i = 0; i < numNewMessages; i++) {
    String chat_id = String(bot.messages[i].chat_id);
    String text = bot.messages[i].text;
    String from_name = bot.messages[i].from_name;

    Serial.print("Message from ");
    Serial.print(from_name);
    Serial.print(" (");
    Serial.print(chat_id);
    Serial.print(") ");
    Serial.println(text);

    if (chat_id != CHAT_ID){
      //bot.sendMessage(chat_id, "Неизвестный пользователь", "");
      continue;
    }

    else if (text == "/start") {
      String replyKeyboardJson = R"([
        ["Настройки⚙️", "Монеты📋"],
        ["Режим работы🔕", "Оповещения⏰"]
      ])";

      bot.sendMessageWithReplyKeyboard(chat_id, "Добро подаловать!\nПриятного использования нашим ботом.", "", replyKeyboardJson, true);
    }

    else if (text == "Настройки⚙️"){
      String inlineKeyboardJson1 = R"([
        [{ "text": "Точность изменения", "callback_data": "precision" }, { "text": "Валюта курса", "callback_data": "currency_rate" }],
        [{ "text": "Размер тикера", "callback_data": "ticker_size" }, { "text": "Размер курса", "callback_data": "course_size" }],
        [{ "text": "Помощь", "callback_data": "help" }]
      ])";
      // Отправляем сообщение с inline клавиатурой
      bot.sendMessageWithInlineKeyboard(chat_id, "Меню настроек", "", inlineKeyboardJson1);
    }

    else if (text == "Режим работы🔕") {
      String inlineKeyboardJson = R"([
        [{ "text": "Сон", "callback_data": "sleep" }, { "text": "Гиперсон", "callback_data": "hypersleep" }],
        [{ "text": "Включить", "callback_data": "on" }, { "text": "Беззвучный режим", "callback_data": "volume" }]
      ])";
      // Отправляем сообщение с inline клавиатурой
      bot.sendMessageWithInlineKeyboard(chat_id, "Выберите режим работы парсера\nРежим на данный момент: " + String(SleepMode ? "Сон" : HypersleepMode ? "Гиперсон" : "Включен"), "", inlineKeyboardJson);
    }

    else if (text == "help"){
      bot.sendMessage(chat_id, "Инструкция по работе с парсером ->\n https://telegra.ph/\nПри обнаружении неисправностей пишите на почту:\nnap6890.bar@gmail.com", "");
    }

    else if (text == "Монеты📋") {
      update = false;
      bot.sendMessage(chat_id, "Введите название или тикер монеты, которую хотите увидеть.\nПример: 'Bitcoin' или 'BTC', 'Ethereum' или 'ETH'\nБерите название или тикеры монет с биржи coingecko.com", "");
    }

    else if (text == "up"){
      String inlineKeyboardJson6 = R"([
        [{ "text": "Отмена", "callback_data": "back"}]
      ])";

      update = true;
      UpOrDown = true;

      bot.sendMessageWithInlineKeyboard(chat_id, "Введите одно число для цены на повышение\nКогда цена выбранной монеты превысит введенное число, придет уведомление", "", inlineKeyboardJson6);
    } 

    else if (text == "down"){
      String inlineKeyboardJson6 = R"([
        [{ "text": "Отмена", "callback_data": "back"}]
      ])";

      update = true;
      UpOrDown = false;

      bot.sendMessageWithInlineKeyboard(chat_id, "Введите одно число для цены на понижение\nКогда цена выбранной монеты будет ниже введенного числа, придет уведомление", "", inlineKeyboardJson6);
    }

    else if (text == "back"){
      update = false;
      bot.sendMessage(chat_id, "Настройка оповещений выключена", "");
    }

    else if (text == "rest") {
      priceUpAlert = -1;
      priceDownAlert = -1;
      bot.sendMessage(chat_id, "Все оповещения сброшены.", "");
    }

    else if (text == "alert") {
      String message = "Текущие оповещения:\n";
      if (priceUpAlert > 0) {
        message += "Повышение: " + priceUpAlertStr + "\n";
      }
      if (priceDownAlert > 0) {
        message += "Понижение: " + priceDownAlertStr + "\n";
      }
      if (priceUpAlert == -1 && priceDownAlert == -1) {
        message = "Нет активных оповещений.";
      }
      bot.sendMessage(chat_id, message, "");
    }

    else if (text == "Оповещения⏰"){
      if (Ticker == "Undefined"){
        bot.sendMessage(chat_id, "Для работы с этим меню необходимо выбрать монету в меню 'Монеты'", "");
      }
      else{
        String inlineKeyboardJson5 = R"([
          [{ "text": "На повышение", "callback_data": "up" }, { "text": "На понижение", "callback_data": "down" }],
          [{ "text": "Сбросить", "callback_data": "rest" }, { "text": "Мои оповещения", "callback_data": "alert" }]
        ])";
        bot.sendMessageWithInlineKeyboard(chat_id, "Выберите тип оповещения\nПри смене монеты или валюты оповещения сбрасывются", "", inlineKeyboardJson5);
      }
    }

    else if (text == "volume"){
      String inlineKeyboardJson7 = R"([
        [{ "text": "Включить", "callback_data": "vol_on"}, { "text": "Выключить", "callback_data": "vol_off"}]
      ])";

      bot.sendMessageWithInlineKeyboard(chat_id, "Вы можете выключить уведомления об оповещениях\nУведомления на данный момент: " + String(vol ? "Включены" : "Выключены"), "", inlineKeyboardJson7);
    }

    else if (text == "vol_on"){
      vol = true;
      bot.sendMessage(chat_id, "Уведомления включены", "");
    }

    else if (text == "vol_off"){
      vol = false;
      bot.sendMessage(chat_id, "Уведомления выключены. Не забудьте их включиь, чтобы оповещения работали", "");
    }

    else if (text == "course_size"){
      String inlineKeyboardJson4 = R"([
        [{ "text": "1", "callback_data": "one_course" }, { "text": "2", "callback_data": "two_course" }, { "text": "3", "callback_data": "three_course" }]
      ])";

      bot.sendMessageWithInlineKeyboard(chat_id, "Выберите размер курса\nРазмер курса на данный момент: " + String(PriceSize), "", inlineKeyboardJson4);
    }

    else if (text == "one_course"){
      PriceSize = 1;
      DisplayPrint(Ticker, Price_display, DayPrice_display, TickerSize, PriceSize);
      bot.sendMessage(chat_id, "Выбран размер курсе 1", "");
    }

    else if (text == "two_course"){
      PriceSize = 2;
      DisplayPrint(Ticker, Price_display, DayPrice_display, TickerSize, PriceSize);
      bot.sendMessage(chat_id, "Выбран размер курса 2", "");
    }

    else if (text == "three_course"){
      PriceSize = 3;
      DisplayPrint(Ticker, Price_display, DayPrice_display, TickerSize, PriceSize);
      if(Price_display.length() >= 8){
        bot.sendMessage(chat_id, "Невозможно выбрать размер 3. Ограничение 2", "");
      }
      else{
        bot.sendMessage(chat_id, "Выбран размер курса 3", "");
      }
    }

    else if (text == "ticker_size"){
      String inlineKeyboardJson3 = R"([
        [{ "text": "1", "callback_data": "one_ticker" }, { "text": "2", "callback_data": "two_ticker" }]
      ])";

      bot.sendMessageWithInlineKeyboard(chat_id, "Выберите размер тикера\nРазмер тикера на данный момент: " + String(TickerSize), "", inlineKeyboardJson3);
    }

    else if (text == "one_ticker"){
      TickerSize = 1;
      DisplayPrint(Ticker, Price_display, DayPrice_display, TickerSize, PriceSize);
      bot.sendMessage(chat_id, "Выбран размер тикера 1", "");
    }

    else if (text == "two_ticker"){
      TickerSize = 2;
      DisplayPrint(Ticker, Price_display, DayPrice_display, TickerSize, PriceSize);
      bot.sendMessage(chat_id, "Выбран размер тикера 2", "");
    }

    else if (text == "currency_rate"){
      String inlineKeyboardJson2 = R"([
        [{ "text": "Рубли 'RUB'", "callback_data": "rubles" }, { "text": "Доллары 'USD'", "callback_data": "dollars" }],
        [{ "text": "Евро 'EUR'", "callback_data": "euros" }, { "text": "Фунт стерлингов 'GBP'", "callback_data": "pounds" }],
        [{ "text": "Юань 'CNY'", "callback_data": "yuan" }, { "text": "Иены 'JPY'", "callback_data": "yen" }]
      ])";
      // Отправляем сообщение с inline клавиатурой
      bot.sendMessageWithInlineKeyboard(chat_id, "Выберите валюту\nВыбранная валюта на данный момент: " + CurrencyRate, "", inlineKeyboardJson2);
    }

    else if (text == "yen"){
      CurrencyRate = "JPY";
      SymbolCoin = "";
      price_update = true;
      priceUpAlert = -1;
      priceDownAlert = -1;
      bot.sendMessage(chat_id, "Вы выбрали иену", "");
    }

    else if (text == "yuan"){
      CurrencyRate = "CNY";
      SymbolCoin = "";
      price_update = true;
      priceUpAlert = -1;
      priceDownAlert = -1;
      bot.sendMessage(chat_id, "Вы выбрали юань", "");
    }

    else if (text == "pounds"){
      CurrencyRate = "GBP";
      SymbolCoin = "";
      price_update = true;
      priceUpAlert = -1;
      priceDownAlert = -1;
      bot.sendMessage(chat_id, "Вы выбрали фунт стерлингов", "");
    }

    else if (text == "euros"){
      CurrencyRate = "EUR";
      SymbolCoin = "";
      price_update = true;
      priceUpAlert = -1;
      priceDownAlert = -1;
      bot.sendMessage(chat_id, "Вы выбрали валюту евро", "");
    }

    else if (text == "rubles"){
      CurrencyRate = "RUB";
      SymbolCoin = "";
      price_update = true;
      priceUpAlert = -1;
      priceDownAlert = -1;
      bot.sendMessage(chat_id, "Вы выбрали валюту рубль", "");
    }

    else if (text == "dollars"){
      CurrencyRate = "USD";
      SymbolCoin = "$";
      price_update = true;
      priceUpAlert = -1;
      priceDownAlert = -1;
      bot.sendMessage(chat_id, "Вы выбрали валюту доллар", "");
    }

    else if (text == "precision"){
      String inlineKeyboardJson2 = R"([
        [{ "text": "1", "callback_data": "one_precision" }, { "text": "2", "callback_data": "two_precision" }],
        [{ "text": "3", "callback_data": "three_precision" }, { "text": "4", "callback_data": "four_precision" }]
      ])";
      // Отправляем сообщение с inline клавиатурой
      bot.sendMessageWithInlineKeyboard(chat_id, "Выберите точность дневного изменения\nВыбранная точность на данный момент: " + String(precision_change), "", inlineKeyboardJson2);
    }

    else if (text == "one_precision"){
      precision_change = 1;
      DisplayPrint(Ticker, Price_display, DayPrice_display, TickerSize, PriceSize);
      bot.sendMessage(chat_id, "Установлена точность 1", "");
    }

    else if (text == "two_precision"){
      precision_change = 2;
      DisplayPrint(Ticker, Price_display, DayPrice_display, TickerSize, PriceSize);
      bot.sendMessage(chat_id, "Установлена точность 2", "");
    }

    else if (text == "three_precision"){
      precision_change = 3;
      DisplayPrint(Ticker, Price_display, DayPrice_display, TickerSize, PriceSize);
      bot.sendMessage(chat_id, "Установлена точность 3", "");
    }

    else if (text == "four_precision"){
      precision_change = 4;
      DisplayPrint(Ticker, Price_display, DayPrice_display, TickerSize, PriceSize);
      bot.sendMessage(chat_id, "Установлена точность 4", "");
    }

    else if (text == "sleep"){
      bot.sendMessage(chat_id, "Включен режим сна", "");
      display.clearDisplay();
      display.display();
      SleepMode = true;
    }

    else if (text == "hypersleep"){
      SleepMode = false;
      HypersleepMode = true;
      bot.sendMessage(chat_id, "Включен режим гиперсна", "");
      display.clearDisplay();
      display.setTextSize(2);
      printCenter("Hypersleep", 0, 27);
      display.display();
      delay(5000);
      display.clearDisplay();
      display.display();
    }

    else if(text == "on"){
      bot.sendMessage(chat_id, "Режим сна выключен", "");
      DisplayPrint(Ticker, Price_display, DayPrice_display, TickerSize, PriceSize);
      SleepMode = false;
      HypersleepMode = false;
    }

    else{
      if(!HypersleepMode){
        if (update){
          UpdateAlert(text, chat_id);
        }
        else{
          getCoinData(text);
          if(!CoinSearch){
            bot.sendMessage(chat_id, "Нераспознанная команда или некорректное название монеты", "");
          }
          else{
            bot.sendMessage(chat_id, "Показан курс " + Ticker, "");
            price_update = true;
          }
        }
      }
      else{
        bot.sendMessage(chat_id, "Команда нераспознана. Выключен режим гиперсна.\nДля его отключения нажмите на кнопку 'Включить' в меню 'Режим работы'", "");
      }
    }
  }
}

// Функция настройки уведомлений
void UpdateAlert(String text, String chat_id) {
  float alertPrice = text.toFloat();
  
  // Проверяем, удалось ли корректно преобразовать текст в число и находится ли оно в допустимом диапазоне
  if (alertPrice > 0 && alertPrice <= 100000000) {
    String formattedPrice;
    
    // Проверяем, содержит ли исходный текст дробные числа
    if (text.indexOf('.') != -1) {
      // Если текст содержит '.', выводим число с полной точностью
      formattedPrice = text;
    } 
    else {
      // Если текст не содержит '.', выводим число как целое
      formattedPrice = String(alertPrice, 0);
    }
    
    if (UpOrDown) {
      priceUpAlert = alertPrice;
      priceUpAlertStr = formattedPrice;
      bot.sendMessage(chat_id, "Оповещение на повышение цены установлено: " + formattedPrice, "");
    } 
    else {
      priceDownAlert = alertPrice;
      priceDownAlertStr = formattedPrice;
      bot.sendMessage(chat_id, "Оповещение на понижение цены установлено: " + formattedPrice, "");
    }
    update = false;
  } 
  else {
    bot.sendMessage(chat_id, "Некорректная цена. Введите цену от 0 до 100 000 000.", "");
  }
}

// Функция проверки уведомлеинй
void MyCalls() {
  // Преобразуем Price_display в число с плавающей запятой
  Serial.println(coin);
  Serial.println(priceUpAlert);
  
  // Проверяем, достигла ли цена порога для повышения
  if (coin >= priceUpAlert && priceUpAlert > 0) {
    bot.sendMessage(CHAT_ID, "Цена достигла порога для повышения: " + Price_display + " " + CurrencyRate + "\nОповещение выключено", "");
    priceUpAlert = -1;  // Сброс оповещения
  }

  // Проверяем, достигла ли цена порога для понижения
  if (coin <= priceDownAlert && priceDownAlert > 0) {
    bot.sendMessage(CHAT_ID, "Цена достигла порога для понижения: " + Price_display + " " + CurrencyRate + "\nОповещение выключено", "");
    priceDownAlert = -1;  // Сброс оповещения
  }
}

void loop() {
  server.handleClient();
  if (WiFi.status() == WL_CONNECTED) {
    t = millis();
    while(millis() - t <= interval && price_update == false){
      if (millis() - lastTimeBotRan > 1000) {
        int numNewMessages = bot.getUpdates(bot.last_message_received + 1);
        while (numNewMessages) {
          handleNewMessages(numNewMessages);
          numNewMessages = bot.getUpdates(bot.last_message_received + 1);
        }
        lastTimeBotRan = millis();
      }
    }
    if(!HypersleepMode){
      Serial.println("Начало отправки");
      CoinData(URL.c_str());
      if (vol){
        MyCalls();
      }
      Serial.print("Цена: ");
      Serial.println(Price_display);
      Serial.print("Изменение: ");
      Serial.println(DayPrice_display, 4);
      if(!SleepMode){
        DisplayPrint(Ticker, Price_display, DayPrice_display, TickerSize, PriceSize);
        price_update = false;
      }
      Serial.println("Конец отправки");
    }
  }
}

// Функция центрирования текста
void printCenter(const String buf, int x, int y){
  int16_t x1, y1;
  uint16_t w, h;
  display.getTextBounds(buf, x, y, &x1, &y1, &w, &h);                     
  display.setCursor((x - w / 2) + (128 / 2), y);                          
  display.print(buf);                                                     
}

// Функция печати на дисплей
void DisplayPrint(const String coin, const String price, float change, int s_coin, int s_price){
  display.clearDisplay();
  display.setTextSize(s_coin);
  if (coin != "Undefined"){
    if (CurrencyRate == "USD"){
      printCenter("USDT/" + String(coin), 0, 0);
    }
    else{
      printCenter(CurrencyRate + "/" + String(coin), 0, 0);
    }
  }
  else{
    printCenter("Undefined", 0, 0);
  }

  if(price.length() >= 8 && s_price == 3){
    PriceSize = 2;
    display.setTextSize(2);
  }
  else{
    display.setTextSize(s_price);
  }
  printCenter(price, 0, 25);

  display.setTextSize(1);

  String changeStr = (change >= 0 ? "+" : "") + String(change, precision_change);

  printCenter("24hr.Change " + changeStr + "%", 0, 55);

  display.display();
  Serial.println("Обновление дисплея");
}