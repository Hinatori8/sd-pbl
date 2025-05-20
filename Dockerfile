# ─────────────────────────────────────────────────────────────
# Dockerfile  (SD_PBL Web & CGI サーバー)
# ─────────────────────────────────────────────────────────────
FROM debian:stable-slim

# 1) Apache とビルドツールをインストール
RUN apt-get update && \
    apt-get install -y apache2 apache2-dev gcc make && \
    rm -rf /var/lib/apt/lists/*

# 2) CGI モジュールを有効化
RUN a2enmod cgi

# 3) アプリ用ディレクトリを用意
RUN mkdir -p /var/www/html /var/www/html/cgi-bin

# 4) プロジェクト全体をドキュメントルート直下へコピー
COPY . /var/www/html

# 5) CGI スクリプトをビルド・実行権付与
WORKDIR /var/www/html/cgi-bin
RUN chmod +x *.cgi && ./compile_cgis.sh

# 6) Debian デフォルトのウェルカムページを削除（任意）
RUN rm -f /var/www/html/index.html

# 7) CGI 用 Apache 設定を追加
RUN printf '\
ScriptAlias /cgi-bin/ /var/www/html/cgi-bin/\n\
<Directory "/var/www/html/cgi-bin">\n\
    Options +ExecCGI\n\
    AddHandler cgi-script .cgi\n\
    Require all granted\n\
</Directory>\n' \
  > /etc/apache2/conf-available/enable-cgi.conf && \
    a2enconf enable-cgi

# 8) 公開ポート
EXPOSE 80

# 9) コンテナ起動時に Apache をフォアグラウンド起動
CMD ["apachectl", "-D", "FOREGROUND"]
