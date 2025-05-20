# ベースイメージに Debian slim を指定
FROM debian:stable-slim

# Apache, ビルドに必要なツールをインストール
RUN echo "\
ScriptAlias /cgi-bin/ /var/www/html/cgi-bin/\n\
<Directory \"/var/www/html/cgi-bin\">\n\
    Options +ExecCGI\n\
    AddHandler cgi-script .cgi\n\
    Require all granted\n\
</Directory>" \
>> /etc/apache2/sites-enabled/000-default.conf
# プロジェクトの全ファイルを /var/www 配下へコピー
COPY . /var/www/html

# 作業ディレクトリを cgi-bin に設定
WORKDIR /var/www/html/cgi-bin

# CGI スクリプトに実行権限を付与し、ビルドスクリプトを実行
RUN chmod +x *.cgi && ./compile_cgis.sh

# Apache を 8080 ポートで待ち受け
EXPOSE 8080

# コンテナ起動時に Apache をフォアグラウンドで起動
CMD ["apachectl", "-D", "FOREGROUND"]
