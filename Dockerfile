# ─────────────────────────────────────────────────────────────
# Dockerfile  – SD_PBL 予約システム (Apache + CGI + データ保存)
# ─────────────────────────────────────────────────────────────
FROM debian:stable-slim

# 1) 必要パッケージをインストール
RUN apt-get update && \
    apt-get install -y apache2 apache2-dev gcc make && \
    rm -rf /var/lib/apt/lists/*

# 2) CGI モジュールを有効化
RUN a2enmod cgi

# 3) ドキュメントルートを用意
RUN mkdir -p /var/www/html/cgi-bin /var/www/html/data

# 4) プロジェクト全体をドキュメントルート配下へコピー
#    - index.html / images / cgi-bin / data などすべてが反映される
COPY . /var/www/html

# 5) CGI スクリプトをビルド & 実行権付与
WORKDIR /var/www/html/cgi-bin
RUN chmod +x *.cgi && \
    if [ -f ./compile_cgis.sh ]; then ./compile_cgis.sh; fi

# 6) Apache に CGI 用設定を追加
RUN printf '\
ScriptAlias /cgi-bin/ /var/www/html/cgi-bin/\n\
<Directory "/var/www/html/cgi-bin">\n\
    Options +ExecCGI -Indexes\n\
    AddHandler cgi-script .cgi\n\
    Require all granted\n\
</Directory>\n' \
  > /etc/apache2/conf-available/enable-cgi.conf && \
  a2enconf enable-cgi

# 7) 予約データ用ディレクトリに www-data が書き込み可能な権限を付与
RUN chown -R www-data:www-data /var/www/html/data

# 8) 公開ポート
EXPOSE 80

# 9) Apache をフォアグラウンドで起動
CMD ["apachectl", "-D", "FOREGROUND"]
