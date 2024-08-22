import requests


def fetch_coins(page, per_page):
    url = 'https://api.coingecko.com/api/v3/coins/markets'
    params = {
        'vs_currency': 'usd',
        'order': 'market_cap_desc',
        'per_page': per_page,
        'page': page
    }
    response = requests.get(url, params=params)
    response.raise_for_status()  # Проверка успешно
    return response.json()


# Получение монет
coins = []
per_page = 100
pages_needed = 3  # 3 страницы для 300 монет

for page in range(1, pages_needed + 1):
    coins.extend(fetch_coins(page, per_page))

# Проверка 300 монет
if len(coins) != 300:
    raise ValueError(f"Не удалось получить 300 монет. Получено монет: {len(coins)}")

# Создаем строки для C++ массива
cpp_array = []
for coin in coins:
    name = coin['name'].upper()
    api_url = f"/api/v3/simple/price?ids={coin['id']}&vs_currencies=%CURRENCY%&include_24hr_change=true"
    symbol = coin['symbol'].upper()
    cpp_array.append(f'{{"{name}", "{api_url}", "{symbol}"}}')

# Печатаем массив в формате C++
print(",\n".join(cpp_array))
