"""
Python 3
Get the current value of a Bitcoin in US dollars using the bitcoincharts api
"""
import urllib.request
import json
import time
import requests
def get_btc():
    """
    Get the current value of a Bitcoin in US dollars using the bitcoincharts api
    """
    url = "http://api.bitcoincharts.com/v1/weighted_prices.json"
    response = requests.get(url)
    data = response.json()
    return data["USD"]["24h"]   