# ベースイメージに Debian slim を指定
FROM debian:stable-slim

# Apache, ビルドに必要なツールをインストール
RUN apt-get update && \
    apt-get install -y apache2 apache2-dev gcc make && \
    a2enmod cgi && \
    mkdir -p /var/www/html /var/www/cgi-bin

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
