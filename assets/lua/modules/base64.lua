local b64 = dofile('base64/mime_base64.lua')

return {
  encode = b64.base64_encode,
  decode = b64.base64_decode
}
