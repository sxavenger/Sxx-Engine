# Cache Management
## Cacheについて
cacheは, `Assets Cache`と`Component Cache`の2種類に分けられる。

### Static Cache
Assetsからのキャッシュデータ
Assetsに付与してある`Uuid`をキーとしてキャッシュする。

#### Cacheの更新方法
1. uuidをキーとしてキャッシュを取得する
2. キャッシュ側のaddressとAssets側のaddressを比較する
3. 一致していなければ, キャッシュを更新する
4. 一致していれば, キャッシュをそのまま利用する

### Dynamic Cache
Componentからのキャッシュデータ
Componentの`address`をキーとしてキャッシュする。
