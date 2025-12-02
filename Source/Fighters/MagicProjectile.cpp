#include "MagicProjectile.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "EnemyCharacter.h"
#include "GameFramework/DamageType.h"
#include "Components/PrimitiveComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"

AMagicProjectile::AMagicProjectile()
{
	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	CollisionComp->InitSphereRadius(15.0f);
	CollisionComp->SetCollisionProfileName("Projectile");
	CollisionComp->SetNotifyRigidBodyCollision(true);
	CollisionComp->BodyInstance.SetCollisionProfileName("Projectile");

	// setup not to collide with pawns (the caster)
	// CollisionComp->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);

	// set CanMove to true
	CollisionComp->SetMobility(EComponentMobility::Movable);

	RootComponent = CollisionComp;

	CollisionComp->OnComponentHit.AddDynamic(this, &AMagicProjectile::OnProjectileHit);

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComp"));
	ProjectileMovement->UpdatedComponent = CollisionComp;
	ProjectileMovement->InitialSpeed = 10.f;
	ProjectileMovement->MaxSpeed = 10.f;
	ProjectileMovement->bShouldBounce = false;
	ProjectileMovement->bAutoActivate = true;
	ProjectileMovement->ProjectileGravityScale = 0.f;

	// Niagara effect component
	EffectComp = CreateDefaultSubobject<UNiagaraComponent>(TEXT("EffectComp"));
	EffectComp->SetupAttachment(RootComponent);
}

void AMagicProjectile::BeginPlay()
{
	Super::BeginPlay();

	// Ensure we ignore the owner actor to prevent immediate self-collision
	if (AActor *MyOwner = GetOwner())
	{
		if (CollisionComp)
		{
			CollisionComp->IgnoreActorWhenMoving(MyOwner, true);
		}
	}
}

// Check if a location vector is valid and within safe bounds
static bool IsValidSafeLocation(const FVector &V)
{
	const float MaxAbs = 1e7f;
	return FMath::IsFinite(V.X) && FMath::IsFinite(V.Y) && FMath::IsFinite(V.Z) && FMath::Abs(V.X) <= MaxAbs && FMath::Abs(V.Y) <= MaxAbs && FMath::Abs(V.Z) <= MaxAbs;
}

void AMagicProjectile::InitializeSpell(const FFinalSpellData &NewData)
{
	SpellData = NewData;
	UE_LOG(LogTemp, Log, TEXT("AMagicProjectile::InitializeSpell - Spell initialized with Damage: %f, Speed: %f, Duration: %f"), SpellData.FinalDamage, SpellData.FinalSpeed, SpellData.FinalDuration);
	// debug Log: speed check
	if (!FMath::IsFinite(NewData.FinalSpeed))
	{
		UE_LOG(LogTemp, Warning, TEXT("AMagicProjectile::InitializeSpell - FinalSpeed is not finite"));
	}

	// setup projectile movement based on spell data
	float UseSpeed = NewData.FinalSpeed;
	ProjectileMovement->MaxSpeed = UseSpeed;
	ProjectileMovement->InitialSpeed = UseSpeed;

	ProjectileMovement->Activate(true);

	// setup lifespan
	SetLifeSpan(NewData.FinalDuration > 0 ? NewData.FinalDuration : 3.0f);
}

void AMagicProjectile::OnProjectileHit(UPrimitiveComponent *HitComp, AActor *OtherActor, UPrimitiveComponent *OtherComp, FVector NormalImpulse, const FHitResult &Hit)
{
	if (!OtherActor || OtherActor == this || OtherActor == GetOwner())
		return;

	// Apply knockback before damage
	if (AEnemyCharacter* Enemy = Cast<AEnemyCharacter>(OtherActor))
	{
		if (SpellData.FinalKnockbackForce > 0.f)
		{
			FVector KnockbackDir = GetActorForwardVector();
			Enemy->Knockback(KnockbackDir, SpellData.FinalKnockbackForce);
		}
	}
	// Hit logic
	if (SpellData.FinalAOERange > 0.f)
	{
		// Apply radial damage if AOE range is set
		UE_LOG(LogTemp, Verbose, TEXT("AMagicProjectile::OnProjectileHit - Applying %f radial damage with radius %f at %s"), SpellData.FinalDamage, SpellData.FinalAOERange, *Hit.Location.ToString());
		TArray<AActor*> IgnoredActors;
		IgnoredActors.Add(this);
		if (GetOwner())
		{
			IgnoredActors.Add(GetOwner());
		}
		
		UGameplayStatics::ApplyRadialDamage(GetWorld(), SpellData.FinalDamage, Hit.Location, SpellData.FinalAOERange, UDamageType::StaticClass(), IgnoredActors, this, GetInstigatorController(), true);
	}
	else
	{
		// Apply direct damage if no AOE
		UE_LOG(LogTemp, Verbose, TEXT("AMagicProjectile::OnProjectileHit - Applying %f damage to %s"), SpellData.FinalDamage, *OtherActor->GetName());
		UGameplayStatics::ApplyDamage(OtherActor, SpellData.FinalDamage, GetInstigatorController(), this, UDamageType::StaticClass());
	}
	
	// spawn impact effects

	if (SpellData.ImpactFX)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), SpellData.ImpactFX, Hit.Location, Hit.ImpactNormal.Rotation());
	}
	if (SpellData.ImpactSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, SpellData.ImpactSound, Hit.Location);
	}

	// Stop movement and disable further collisions
	ProjectileMovement->StopMovementImmediately();
	CollisionComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// Visualize AOE range by scaling up
	if (SpellData.FinalAOERange > 0.f)
	{
		// We scale the whole actor. The Niagara component will scale with it.
		SetActorScale3D(FVector(SpellData.FinalAOERange / CollisionComp->GetUnscaledSphereRadius()));
	}

	// Destroy the actor after a short delay to show the effect
	SetLifeSpan(0.2f);
}