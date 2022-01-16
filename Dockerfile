from golang as gobuild

RUN mkdir /app
COPY levelbuilder /app/levelbuilder
COPY slidebuilder /app/slidebuilder

WORKDIR /app

RUN go env -w GO111MODULE=off
RUN go build -o levelbuilder/levelbuilder ./levelbuilder/
RUN go build -o slidebuilder/slidebuilder ./slidebuilder/

from lambertjamesd/modern-n64sdk as telocation

RUN apt install libnustd

COPY --from=gobuild /app/levelbuilder/levelbuilder levelbuilder/levelbuilder
COPY --from=gobuild /app/slidebuilder/slidebuilder slidebuilder/slidebuilder

COPY asm asm
COPY bin bin
COPY collision collision
COPY imageslides imageslides
COPY levelbuilder/ply levelbuilder/ply
COPY levels levels
COPY Makefile Makefile
COPY sound/clips sound/clips
COPY sound/ins sound/ins
COPY sound/music sound/music
COPY src src
COPY telocation.ld telocation.ld

CMD make output/telocation.z64