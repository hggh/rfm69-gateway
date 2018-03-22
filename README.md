# rfm69-gateway
RFM69 Gateway 433Mhz &lt;=> openHAB2


## message format

  <NodeID>;Command;Value

# message examples

  shutter down:
  
  49;DOWN
  
  shutter up:
  
  49;UP
  
  light on
  
  49;L_ON
  
  receive DHT22 values from Node 50
  
  [50] 50;20.70;29.20[RX_RSSI:-36]
